#include "qdiscord.h"

#include <QJsonDocument>
#include <QTimer>

struct MessageHeader {
	uint32_t opcode;
	uint32_t length;
};

QDiscord::QDiscord() {
	QObject::connect(&socket_, &QLocalSocket::errorOccurred, this, [this](const QLocalSocket::LocalSocketError &err) {
		qWarning() << "QDiscord socket error: " << static_cast<int>(err);
	});
}

bool QDiscord::connect(qint64 clientID) {
	// start connecting
	socket_.connectToServer("discord-ipc-0");
	qDebug() << "Trying to connect to Discord";

	if(!socket_.waitForConnected(3000)) {
		qDebug() << "Connection failed";
		return false;
	}

	qDebug() << "Connected.";

	// Handshake command
	{
		qDebug() << "Sending handshake";
		sendMessage(QJsonObject{
			{"v",         1},
			{"client_id", clientID}
		}, 0);
	}

	// Receive DISPATCH
	{
		const QJsonObject msg = readMessage();
		qDebug() << "RCVD MSG";
		qDebug().noquote() << QJsonDocument(msg).toJson();
	}

	qDebug() << "Connection successful";
	return true;
}

void QDiscord::disconnect() {
	socket_.disconnectFromServer();
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
	return result;
}

void QDiscord::sendMessage(const QJsonObject &packet, int opCode) {
	const QByteArray payload = QJsonDocument(packet).toJson(QJsonDocument::Compact);

	qDebug() << "Send" << packet;

	MessageHeader h;
	h.opcode = static_cast<uint32_t>(opCode);
	h.length = payload.length();

	socket_.write(reinterpret_cast<const char *>(&h), sizeof(MessageHeader));
	socket_.write(payload);
}

QByteArray QDiscord::blockingReadBytes(int bytes) {
	while(socket_.bytesAvailable() < bytes) {
		if(!socket_.waitForReadyRead(3000)) {
			qWarning() << "QDiscord - waitForReadyRead timeout";
			return {};
		}
	}

	return socket_.read(bytes);
}
