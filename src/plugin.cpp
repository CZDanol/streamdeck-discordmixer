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
		connect(&deck, &QStreamDeckPlugin::sendToPlugin, this, [this](const QStreamDeckAction &action) {
			devices_[action.deviceId]->onSendToPlugin(action);
		});

		connect(&deck, &QStreamDeckPlugin::keyDown, this, [this](const QStreamDeckAction &action) {
			if(auto btn = devices_[action.deviceId]->buttons.value(action.context))
				btn->onPressed();
		});
		connect(&deck, &QStreamDeckPlugin::keyUp, this, [this](const QStreamDeckAction &action) {
			if(auto btn = devices_[action.deviceId]->buttons.value(action.context))
				btn->onReleased();
		});
	}

	return true;
}