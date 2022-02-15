#pragma once

#include <QString>
#include <QScopedPointer>
#include <QSharedPointer>
#include <QCache>

#include <qtstreamdeck/qstreamdeckplugin.h>
#include <qtdiscordipc/qdiscord.h>

#include "device.h"

struct ESDConfig {
	int port;
	QString pluginUUID;
	QString registerEvent;
	QString info;
};

class Plugin : public QObject {
	Q_OBJECT

public:
	bool init(const ESDConfig &esdConfig);
	bool connectToDiscord();

public:
	void subscribeVoiceEvents(const QString &channelId);

	/// Safely obtains device, creates it when it doesn't exist (even though it should, but
	Device *obtainDevice(const QString &id);

	void updateButtons();

public:
	QStreamDeckPlugin deck;
	QDiscord discord;
	QString curentDiscordChannelID;
	QJsonObject globalSettings;

private:
	QHash<QString, QSharedPointer<Device>> devices_;
	QHash<QString, QString> contextDevices_; /// Map context -> deviceID

};

extern Plugin *plugin;
