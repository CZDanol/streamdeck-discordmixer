#pragma once

#include <QObject>
#include <QLocalSocket>
#include <QJsonObject>

class QDiscord : public QObject {
	Q_OBJECT

public:
	QDiscord();

public:
	/**
	 * Tries to connext to the Discord. Returns true if successfull (this function is blocking)
	 */
	bool connect(qint64 clientID);

	void disconnect();

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

