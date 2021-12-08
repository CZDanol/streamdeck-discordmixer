#pragma once

#include <QString>
#include <QScopedPointer>
#include <QSharedPointer>

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

public:
	void subscribeVoiceEvents(const QString &channelId);

public:
	QStreamDeckPlugin deck;
	QDiscord discord;

private:
	QHash<QString, QSharedPointer<Device>> devices_;
	QHash<QString, QString> contextDevices_; /// Map context -> deviceID

};

extern Plugin *plugin;
