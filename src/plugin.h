#pragma once

#include <QString>
#include <QScopedPointer>

#include <ElgatoStreamDeck/esdplugin.h>

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

private slots:
	void onKeyDown(const ESDActionModel &action);

private:
	QScopedPointer<ESDPluginBase> esd_;

};
