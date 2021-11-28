#include "plugin.h"

#include "button/button.h"

bool Plugin::init(const ESDConfig &esdConfig) {
	// Init Stream Deck
	{
		deck.init(esdConfig.port, esdConfig.pluginUUID, esdConfig.registerEvent, esdConfig.info);
		if(!deck.connect())
			return false;

		connect(&deck, &QStreamDeckPlugin::deviceDidConnect, this, [this](const QString &deviceID) {
			devices_[deviceID].reset(new Device(*this, deviceID));
		});
		connect(&deck, &QStreamDeckPlugin::deviceDidDisconnect, this, [this](const QString &deviceID) {
			devices_.remove(deviceID);
		});

		connect(&deck, &QStreamDeckPlugin::willAppear, this, [this](const QStreamDeckAction &action) {
			devices_[action.deviceId]->onAppear(action);
		});
		connect(&deck, &QStreamDeckPlugin::willDisappear, this, [this](const QStreamDeckAction &action) {
			devices_[action.deviceId]->onDisappear(action);
		});

		connect(&deck, &QStreamDeckPlugin::keyDown, this, &Plugin::onKeyDown);
	}

	// Init Discord
	{
		if(!discord.connect("914314199436509185", "SzXpJsT64jvZZINODArVirIYY-BkVepl"))
			return false;
	}

	return true;
}

void Plugin::onKeyDown(const QStreamDeckAction &action) {
	if(auto btn = devices_[action.deviceId]->buttons.value(action.context))
		btn->onPressed();
}