#include "qdiscord.h"

#include <QJsonDocument>
#include <QTimer>
#include <QRandomGenerator64>
#include <QFile>
#include <QtNetworkAuth/QOAuth2AuthorizationCodeFlow>
#include <QtNetworkAuth/QOAuthHttpServerReplyHandler>
#include <QHttpMultiPart>
#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QDesktopServices>
#include <QUrlQuery>

struct MessageHeader {
	uint32_t opcode;
	uint32_t length;
};
static_assert(sizeof(MessageHeader) == 8);

QDiscord::QDiscord() {
	QObject::connect(&socket_, &QLocalSocket::errorOccurred, this, [this](const QLocalSocket::LocalSocketError &err) {
		qWarning() << "QDiscord socket error: " << static_cast<int>(err);
	});
}

bool QDiscord::connect(const QString &clientID) {
	const bool r = [&]() {
		// start connecting
		socket_.connectToServer("discord-ipc-0");
		qDebug() << "Trying to connect to Discord";

		if(!socket_.waitForConnected(3000)) {
			qDebug() << "Connection failed";
			return false;
		}

		static const QStringList scopes{"rpc", "identify"};

		// Handshake and dispatch Receive DISPATCH
		{
			sendMessage(QJsonObject{
				{"v",         1},
				{"client_id", clientID},
			}, 0);

			const QJsonObject msg = readMessage();
			if(msg["cmd"] != "DISPATCH") {
				qWarning() << "QDiscord - unexpected message (expected DISPATCH)" << msg["cmd"];
				return false;
			}
		}

		// Send authorization request
		if(true) {
			const QJsonObject msg = sendCommandAndGetResponse("AUTHORIZE", QJsonObject{
				{"client_id", clientID},
				{"scopes",    QJsonArray::fromStringList(scopes)}
			});

			if(msg["cmd"] != "AUTHORIZE") {
				qWarning() << "Authorize - unexpected result" << msg;
				return false;
			}

			const QString authCode = msg["data"].toObject()["code"].toString();

			QOAuth2AuthorizationCodeFlow oauth;

			QEventLoop evl;

			QOAuthHttpServerReplyHandler *replyHandler = new QOAuthHttpServerReplyHandler(1337, this);
			replyHandler->setCallbackPath("callback");

			oauth.setReplyHandler(replyHandler);
			oauth.setAuthorizationUrl(QUrl("https://discord.com/api/oauth2/authorize"));
			oauth.setAccessTokenUrl(QUrl("https://discord.com/api/oauth2/token"));
			oauth.setClientIdentifier(clientID);
			oauth.setModifyParametersFunction([&](QAbstractOAuth::Stage, QMultiMap<QString, QVariant> *parameters) {
				parameters->replace("code", authCode);
				parameters->replace("redirect_uri", "http://localhost:1337/callback");
				parameters->replace("scope", scopes.join(' '));
			});

			QObject::connect(&oauth, &QOAuth2AuthorizationCodeFlow::authorizeWithBrowser, [](const QUrl &url) {
				qDebug() << "Auth with browser";
				QDesktopServices::openUrl(url);
			});
			QObject::connect(&oauth, &QOAuth2AuthorizationCodeFlow::statusChanged, &evl, [&](QAbstractOAuth::Status status) {
				qDebug() << "Status change" << static_cast<int>(status);

				if(status == QAbstractOAuth::Status::Granted)
					evl.quit();
			});
			QObject::connect(&oauth, &QOAuth2AuthorizationCodeFlow::error, &evl, [&](const QString &err, const QString &desc, const QUrl &url) {
				qWarning() << "OAUTH error" << err << desc << url;
				evl.quit();
			});

			oauth.grant();

			// Timeout timer
			QTimer t;
			t.setInterval(300000);
			t.setSingleShot(true);
			t.start();
			QObject::connect(&t, &QTimer::timeout, &evl, &QEventLoop::quit);

			qDebug() << "OAUTH evl start";

			evl.exec();

			if(oauth.status() != QAbstractOAuth::Status::Granted) {
				qWarning() << "Failed OAuth";
				return false;
			}

			qWarning() << "OAUTH SUCCESS";

		/*	QNetworkAccessManager nm;
			QNetworkRequest req;
			const QUrlQuery q{
				{"client_id",     clientID},
				{"client_secret", "code"},
				{"code",          authCode},
				{"scope",         scopes.join(' ')},
				{"grant_type",    "authorization_code"},
			};
			QUrl url("https://discord.com/api/oauth2/token");
			req.setUrl(url);
			req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
			auto r = nm.post(req, q.toString(QUrl::FullyEncoded).toUtf8());

			QEventLoop l;
			QObject::connect(r, &QNetworkReply::finished, &l, &QEventLoop::quit);

			l.exec();

			qDebug() << r->errorString() << r->readAll();
			r->deleteLater();*/

			/*if(!accessToken.isEmpty()) {
				tokenFile.open(QIODevice::WriteOnly);
				tokenFile.write(accessToken.toUtf8());
				tokenFile.close();
			}*/
		}

		// Try reauthenticating if we already have auth token
		QFile tokenFile("discordAccessToken.txt");
		bool isAuthorized = false;
		if(tokenFile.exists()) {
			tokenFile.open(QIODevice::ReadOnly);
			QString accessToken = tokenFile.readAll();
			tokenFile.close();

			if(!accessToken.isEmpty()) {
				const QJsonObject msg = sendCommandAndGetResponse("AUTHENTICATE", QJsonObject{
					{"access_token", accessToken}
				});

				if(msg["CMD"] != "AUTHORIZE")
					return false;

			}
		}

		qDebug() << "Connection successful";
		return true;
	}();

	if(!r)
		disconnect();

	return r;
}

void QDiscord::disconnect() {
	socket_.disconnectFromServer();
}

void QDiscord::sendCommand(const QString &command, const QJsonObject &args) {
	QJsonObject message{
		{"cmd",   command},
		{"args",  args},
		{"nonce", QString::number(QRandomGenerator64::global()->generate())}
	};
	sendMessage(message);
}

QJsonObject QDiscord::readMessage() {
	const QByteArray headerBA = blockingReadBytes(sizeof(MessageHeader));
	if(headerBA.isNull())
		return {};

	const MessageHeader &header = *reinterpret_cast<const MessageHeader *>(headerBA.data());

	const QByteArray data = blockingReadBytes(static_cast<int>(header.length));

	QJsonParseError err;
	QJsonObject result = QJsonDocument::fromJson(data, &err).object();

	if(err.error != QJsonParseError::NoError)
		qWarning() << "QDiscord - failed to parse message\n\n" << data;

	result["opcode"] = static_cast<int>(header.opcode);

	qDebug() << "<<<<< RECV\n" << result << "\n";

	if(result["evt"] == "ERROR")
		return {};

	return result;
}

void QDiscord::sendMessage(const QJsonObject &packet, int opCode) {
	const QByteArray payload = QJsonDocument(packet).toJson(QJsonDocument::Compact);

	qDebug() << ">>>>> SEND\n" << packet << "\n";

	MessageHeader header;
	header.opcode = static_cast<uint32_t>(opCode);
	header.length = static_cast<uint32_t>(payload.length());

	socket_.write(QByteArray::fromRawData(reinterpret_cast<const char *>(&header), sizeof(MessageHeader)) + payload);
}

QByteArray QDiscord::blockingReadBytes(int bytes) {
	while(socket_.bytesAvailable() < bytes) {
		if(!socket_.waitForReadyRead(30000)) {
			qWarning() << "QDiscord - waitForReadyRead timeout";
			return {};
		}
	}

	return socket_.read(bytes);
}
