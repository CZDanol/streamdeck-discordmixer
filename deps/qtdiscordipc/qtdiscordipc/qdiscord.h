#pragma once

#include <QObject>
#include <QLocalSocket>
#include <QJsonObject>
#include <QJsonArray>

class QDiscord : public QObject {
	Q_OBJECT

public:
	QDiscord();

public:
	/**
	 * Tries to connext to the Discord. Returns true if successfull (this function is blocking)
	 */
	bool connect(const QString &clientID);

	void disconnect();

public:
	void sendCommand(const QString &command, const QJsonObject &args);
	inline QJsonObject sendCommandAndGetResponse(const QString &command, const QJsonObject &args) {
		sendCommand(command, args);
		return readMessage();
	}

private:
	/// Blocking reads a packet
	QJsonObject readMessage();

	void sendMessage(const QJsonObject &packet, int opCode = 1);

private:
	/// Blocking waits until given amount of bytes is available
	QByteArray blockingReadBytes(int bytes);

private:
	QLocalSocket socket_;

};

