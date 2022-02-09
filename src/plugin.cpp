#include "plugin.h"

#include "button/button.h"

bool Plugin::init(const ESDConfig &esdConfig) {
	// Init Stream Deck
	{
		deck.init(esdConfig.port, esdConfig.pluginUUID, esdConfig.registerEvent, esdConfig.info);
		if(!deck.connect())
			return false;

		connect(&deck, &QStreamDeckPlugin::deviceDidConnect, this, [this](const QString &deviceID, const QJsonObject &deviceInfo) {
			devices_[deviceID].reset(new Device(*this, deviceID, deviceInfo));
		});
		connect(&deck, &QStreamDeckPlugin::deviceDidDisconnect, this, [this](const QString &deviceID) {
			devices_.remove(deviceID);
		});

		connect(&deck, &QStreamDeckPlugin::willAppear, this, [this](const QStreamDeckAction &action) {
			contextDevices_[action.context] = action.deviceId;
			devices_[action.deviceId]->onAppear(action);
		});
		connect(&deck, &QStreamDeckPlugin::willDisappear, this, [this](const QStreamDeckAction &action) {
			devices_[action.deviceId]->onDisappear(action);
			contextDevices_.remove(action.context);
		});
		connect(&deck, &QStreamDeckPlugin::didReceiveSettings, this, [this](const QStreamDeckAction &action) {
			QStreamDeckAction a = action;
			a.deviceId = contextDevices_.value(action.context);
			if(a.deviceId.isEmpty())
				return;

			devices_[a.deviceId]->onSettingsReceived(a);
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

	// Setup discord
	{
		connect(&discord, &QDiscord::connected, this, [this] {
			discord.sendCommand("SUBSCRIBE", {}, {
				{"evt", "VOICE_CHANNEL_SELECT"}
			});
		});
		connect(&discord, &QDiscord::messageReceived, this, [this](const QJsonObject &msg) {
			for(const auto &d: devices_)
				d->onDiscordMessage(msg);

			const QString evt = msg["evt"].toString();
			if(evt == "VOICE_CHANNEL_SELECT")
				subscribeVoiceEvents(msg["data"]["channel_id"].toString());
		});
	}

	return true;
}

void Plugin::subscribeVoiceEvents(const QString &channelId) {
	static const QStringList events{
		"VOICE_STATE_UPDATE", "VOICE_STATE_CREATE", "VOICE_STATE_DELETE", "SPEAKING_START", "SPEAKING_STOP"
	};

	auto f = [&](const QString &cmd) {
		const QJsonObject args{{"channel_id", curentDiscordChannelID}};
		for(const QString &e: events)
			discord.sendCommand(cmd, args, {{"evt", e}});
	};

	if(!curentDiscordChannelID.isNull())
		f("UNSUBSCRIBE");

	curentDiscordChannelID = channelId;

	if(!curentDiscordChannelID.isNull())
		f("SUBSCRIBE");
}

