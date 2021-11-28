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
	QObject::connect(&socket_, &QLocalSocket::disconnected, this, [this] {
		disconnect();
	});
}

bool QDiscord::connect(const QString &clientID, const QString &clientSecret) {
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

		QJsonObject oauthData;
		QFile oauthFile("discordOauth.json");
		if(oauthFile.exists()) {
			oauthFile.open(QIODevice::ReadOnly);
			oauthData = QJsonDocument::fromJson(oauthFile.readAll()).object();
			oauthFile.close();
		}

		// Send authorization request
		if(oauthData["access_token"].isNull()) {
			const QJsonObject msg = sendCommand("AUTHORIZE", QJsonObject{
				{"client_id", clientID},
				{"scopes",    QJsonArray::fromStringList(scopes)}
			});

			if(msg["cmd"] != "AUTHORIZE") {
				qWarning() << "Authorize - unexpected result" << msg;
				return false;
			}

			const QString authCode = msg["data"].toObject()["code"].toString();

			QNetworkAccessManager nm;
			QNetworkRequest req;
			const QUrlQuery q{
				{"client_id",     clientID},
				{"client_secret", clientSecret},
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

			qDebug() << "AUTH REQ" << req.url() << q.toString();

			l.exec();
			r->deleteLater();

			if(r->error() != QNetworkReply::NoError) {
				qWarning() << "QDiscord Network error" << r->errorString();
				return false;
			}

			oauthData = QJsonDocument::fromJson(r->readAll()).object();
			if(oauthData["access_token"].toString().isEmpty()) {
				qWarning() << "QDiscord failed to obtain access token";
				return false;
			}

			{
				oauthFile.open(QIODevice::WriteOnly);
				oauthFile.write(QJsonDocument(oauthData).toJson(QJsonDocument::Compact));
				oauthFile.close();
			}
		}

		// Authenticate
		{
			const QJsonObject msg = sendCommand("AUTHENTICATE", QJsonObject{
				{"access_token", oauthData["access_token"].toString()}
			});

			if(msg["cmd"] != "AUTHENTICATE") {
				qWarning() << "QDiscord expected AUTHENTICATE";
				return false;
			}

			userID_ = msg["data"]["user"]["id"].toString();
		}

		qDebug() << "Connection successful";
		return true;
	}();

	if(!r)
		disconnect();

	isConnected_ = r;
	return r;
}

void QDiscord::disconnect() {
	socket_.disconnectFromServer();
	isConnected_ = false;
	userID_.clear();
}

QJsonObject QDiscord::sendCommand(const QString &command, const QJsonObject &args) {
	QJsonObject message{
		{"cmd",   command},
		{"args",  args},
		{"nonce", QString::number(QRandomGenerator64::global()->generate())}
	};
	sendMessage(message);

	return readMessage();
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
