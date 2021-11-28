#include "plugin.h"

#include <QDebug>
#include <QFile>

struct ActionType {
	static const inline QString openMixer = "cz.danol.discordmixer.openmixer";
	static const inline QString closeMixer = "cz.danol.discordmixer.closemixer";
	static const inline QString user = "cz.danol.discordmixer.user";
};


bool Plugin::init(const ESDConfig &esdConfig) {
	esd_.reset(new ESDPluginBase(esdConfig.port, esdConfig.pluginUUID, esdConfig.registerEvent, esdConfig.info));
	if(!esd_->connect())
		return false;

	connect(esd_.get(), &ESDPluginBase::keyDown, this, &Plugin::onKeyDown);
	qDebug() << "Plugin initialized";
	return true;
}

void Plugin::onKeyDown(const ESDActionModel &action) {
	const QString a = action.action;

	if(a == ActionType::openMixer)
		esd_->switchProfile(action.deviceId, "Discord Volume Mixer");

	else if(a == ActionType::closeMixer)
		esd_->switchProfile(action.deviceId, "");

	else if(a == ActionType::user) {

		return;
	}
}