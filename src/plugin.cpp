#include "plugin.h"

#include <QDebug>

struct ActionType {
	static const inline QString openMixer = "cz.danol.discordmixer.openmixer";
	static const inline QString closeMixer = "cz.danol.discordmixer.closemixer";
	static const inline QString user = "cz.danol.discordmixer.user";
};


bool Plugin::init(const ESDConfig &esdConfig) {
	// Init Stream Deck
	{
		deck_.init(esdConfig.port, esdConfig.pluginUUID, esdConfig.registerEvent, esdConfig.info);
		if(!deck_.connect())
			return false;

		connect(&deck_, &QStreamDeckPlugin::keyDown, this, &Plugin::onKeyDown);
	}

	// Init Discord
	{
		if(!discord_.connect(914314199436509185))
			return false;
	}

	return true;
}

void Plugin::onKeyDown(const QStreamDeckAction &action) {
	const QString a = action.action;

	if(a == ActionType::openMixer)
		deck_.switchProfile(action.deviceId, "Discord Volume Mixer");

	else if(a == ActionType::closeMixer)
		deck_.switchProfile(action.deviceId, "");

	else if(a == ActionType::user) {

		return;
	}
}