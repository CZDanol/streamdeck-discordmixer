#include "device.h"

#include "plugin.h"

#include "button/button.h"
#include "button/button_open_mixer.h"
#include "button/button_back.h"
#include "button/button_user.h"
#include "button/button_volume.h"

Device::Device(Plugin &plugin, const QString &deviceID) : plugin(plugin), deviceID(deviceID) {

}

Device::~Device() {
	qDeleteAll(buttons);
}

void Device::onAppear(const QStreamDeckAction &action) {
#define CTOR(uid, Class) {uid, [] (const Button::CtorData &d) { return new Class(d); }}

	using CtorF = std::function<Button *(const Button::CtorData &)>;
	static const QHash<QString, CtorF> ctors{
		CTOR("cz.danol.discordmixer.openmixer", Button_OpenMixer),
		CTOR("cz.danol.discordmixer.back", Button_Back),

		CTOR("cz.danol.discordmixer.user", Button_User),
		CTOR("cz.danol.discordmixer.volumeup", Button_Volume),
		CTOR("cz.danol.discordmixer.volumedown", Button_Volume),
	};

	if(auto ctor = ctors.value(action.action)) {
		Button *btn = ctor(Button::CtorData{this, action.action, action.context, action.payload});
		buttons[action.context] = btn;
		btn->update();
	}
}

void Device::onDisappear(const QStreamDeckAction &action) {
	if(auto btn = buttons.take(action.context))
		delete btn;
}

void Device::updateData() {
	const QJsonObject voiceChannelData = plugin.discord.sendCommandAndGetResponse("GET_SELECTED_VOICE_CHANNEL", {});

	voiceStates.clear();
	for(const auto &v: voiceChannelData["data"]["voice_states"].toArray())
		voiceStates += VoiceState::fromJson(v.toObject());

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
