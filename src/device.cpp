#include "device.h"

#include "plugin.h"

#include "button/button.h"
#include "button/button_open_mixer.h"
#include "button/button_back.h"
#include "button/button_user.h"
#include "button/button_volume.h"
#include "button/button_page.h"

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
	};

	auto ctor = ctors.value(d.action);
	if(!ctor)
		return nullptr;

	Button *btn = ctor(d);
	dev->buttons[d.context] = btn;
	btn->update();

	return btn;
}

Device::Device(Plugin &plugin, const QString &deviceID) : plugin(plugin), deviceID(deviceID) {

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
		if(vs.userID != plugin.discord.userID()) {
			voiceStates.insert(vs.userID, vs);
			updateAllButtons();
		}
	}

	else if(evt == "VOICE_STATE_UPDATE") {
		const auto vs = VoiceState::fromJson(msg["data"].toObject());
		if(voiceStates.contains(vs.userID)) {
			voiceStates.insert(vs.userID, vs);
			updateAllButtons();
		}
	}

	else if(evt == "VOICE_STATE_DELETE") {
		const auto vs = VoiceState::fromJson(msg["data"].toObject());
		voiceStates.remove(vs.userID);
		updateAllButtons();
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

	updateAllButtons();
}

void Device::updateAllButtons() {
	for(Button *btn: buttons)
		btn->update();
}

void Device::updateUserRelatedButtons(Device::UserIx userIx) {
	for(auto it = userRelatedButtons.find(userIx), end = userRelatedButtons.end(); it != end && it.key() == userIx; it++)
		it.value()->update();
}
