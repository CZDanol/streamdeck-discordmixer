#include "device.h"

#include "plugin.h"

#include "button/button.h"
#include "button/button_open_mixer.h"
#include "button/button_back.h"
#include "button/button_user.h"
#include "button/button_volume.h"
#include "button/button_page.h"
#include "button/button_microphone.h"
#include "button/button_deafen.h"

Button *createButton(Device *dev, const Button::CtorData &d) {
#define CTOR(uid, Class) {uid, [] (const Button::CtorData &d) { return new Class(d); }}

	using CtorF = std::function<Button *(const Button::CtorData &)>;
	static const QHash<QString, CtorF> ctors{
		CTOR("cz.danol.discordmixer.openmixer", Button_OpenMixer),
		CTOR("cz.danol.discordmixer.back", Button_Back),
		CTOR("cz.danol.discordmixer.nextpage", Button_Page),
		CTOR("cz.danol.discordmixer.previouspage", Button_Page),

		CTOR("cz.danol.discordmixer.user", Button_User),
		CTOR("cz.danol.discordmixer.volumeup", Button_Volume),
		CTOR("cz.danol.discordmixer.volumedown", Button_Volume),

		CTOR("cz.danol.discordmixer.microphone", Button_Microphone),
		CTOR("cz.danol.discordmixer.deafen", Button_Deafen),
	};

	auto ctor = ctors.value(d.action);
	if(!ctor)
		return nullptr;

	Button *btn = ctor(d);
	dev->buttons[d.context] = btn;
	btn->update();

	return btn;
}

Device::Device(Plugin &plugin, const QString &deviceID, const QJsonObject &deviceInfo) : plugin(plugin), deviceID(deviceID), deviceInfo(deviceInfo) {
	connect(&plugin.discord, &QDiscord::avatarReady, this, &Device::updateButtons);
}

Device::~Device() {
	qDeleteAll(buttons);
}

void Device::onAppear(const QStreamDeckAction &action) {
	createButton(this, Button::CtorData{this, action.action, action.context, action.payload});
}

void Device::onDisappear(const QStreamDeckAction &action) {
	delete buttons.take(action.context);
}

void Device::onSendToPlugin(const QStreamDeckAction &action) {
	// Basically recreate the button
	Button *btn = buttons.take(action.context);
	if(!btn)
		return;

	Button::CtorData d{this, action.action, action.context, btn->payload};
	QJsonObject s = d.payload["settings"].toObject();

	for(auto it = action.payload.begin(), end = action.payload.end(); it != end; it++) {
		s[it.key()] = it.value();
	}

	d.payload["settings"] = s;
	createButton(this, d);

	delete btn;
}

void Device::onDiscordMessage(const QJsonObject &msg) {
	if(msg["cmd"] != "DISPATCH")
		return;

	const QString evt = msg["evt"].toString();

	// Update everything when voice channel is changed
	if(evt == "VOICE_CHANNEL_SELECT") {
		updateData();
	}

	else if(evt == "VOICE_STATE_CREATE") {
		const auto vs = VoiceState::fromJson(msg["data"].toObject());
		if(vs.userID == plugin.discord.userID())
			loadSelfVoiceStateUpdate(msg["data"].toObject());

		else {
			voiceStates.insert(vs.userID, vs);
			updateButtons();
		}
	}

	else if(evt == "VOICE_STATE_UPDATE") {
		const auto vs = VoiceState::fromJson(msg["data"].toObject());
		if(vs.userID == plugin.discord.userID())
			loadSelfVoiceStateUpdate(msg["data"].toObject());

		else if(voiceStates.contains(vs.userID)) {
			voiceStates.insert(vs.userID, vs);
			updateButtons();
		}
	}

	else if(evt == "VOICE_STATE_DELETE") {
		const auto vs = VoiceState::fromJson(msg["data"].toObject());
		voiceStates.remove(vs.userID);

		if(userIxOffset >= voiceStates.size())
			userIxOffset = 0;

		updateButtons();
	}

	else if(evt == "SPEAKING_START") {
		const QString userId = msg["data"]["user_id"].toString();
		if(voiceStates.contains(userId)) {
			voiceStates[userId].speaking = true;
			updateButtons();
		}
	}

	else if(evt == "SPEAKING_STOP") {
		const QString userId = msg["data"]["user_id"].toString();
		if(voiceStates.contains(userId)) {
			voiceStates[userId].speaking = false;
			updateButtons();
		}
	}
}

void Device::updateData() {
	const QJsonObject voiceChannelData = plugin.discord.sendCommand("GET_SELECTED_VOICE_CHANNEL", {});

	const QString channelId = voiceChannelData["data"]["id"].toString();
	if(!channelId.isEmpty())
		plugin.subscribeVoiceEvents(channelId);

	voiceStates.clear();
	for(const auto &v: voiceChannelData["data"]["voice_states"].toArray()) {
		const VoiceState vs = VoiceState::fromJson(v.toObject());
		if(vs.userID != plugin.discord.userID())
			voiceStates.insert(vs.userID, vs);
	}

	updateButtons();
}

void Device::updateButtons() {
	for(Button *btn: buttons)
		btn->update();
}

void Device::loadSelfVoiceStateUpdate(const QJsonObject &json) {
	if(auto v = json["voice_state"]["self_mute"]; !v.isNull())
		microphoneMuted = v.toBool();

	if(auto v = json["voice_state"]["self_deaf"]; !v.isNull())
		deafened = v.toBool();

	updateButtons();
}
