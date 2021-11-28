#include "plugin.h"

struct ButtonType {
	static const inline QString openMixer = "cz.danol.discordmixer.openmixer";
	static const inline QString closeMixer = "cz.danol.discordmixer.closemixer";
	static const inline QString user = "cz.danol.discordmixer.user";
};


bool Plugin::init(const ESDConfig &esdConfig) {
	esd_.reset(new ESDPluginBase(esdConfig.port, esdConfig.pluginUUID, esdConfig.registerEvent, esdConfig.info));
	if(!esd_->connect())
		return false;

	connect(esd_.get(), &ESDPluginBase::keyDown, this, &Plugin::onKeyDown);

	return true;
}

void Plugin::onKeyDown(const QString &context, const QString &deviceId, const QJsonObject &payload) {

}