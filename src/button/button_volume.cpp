#include "button_volume.h"

static constexpr int minVolume = 0;
static constexpr int maxVolume = 200;

Button_Volume::Button_Volume(const Button::CtorData &d) : Button_UserRelated(d) {
	step = (d.action == "cz.danol.discordmixer.volumeup" ? 1 : -1) * 5;

	repeatTimer.setInterval(100);
	repeatTimer.callOnTimeout([this] {
		if(repeatSkip-- <= 0)
			trigger();
	});
}

VoiceState *Button_Volume::voiceState() {
	const int ix = effectiveIx();
	if(ix < 0 || ix >= device.voiceStates.size())
		return nullptr;

	return &device.voiceStates[device.voiceStates.keys()[ix]];
}

void Button_Volume::update() {
	VoiceState *s = voiceState();

	const int targetState = s ? (step < 0 && s->volume <= minVolume) || (step > 0 && s->volume >= maxVolume) : 1;
	if(state == targetState)
		return;

	state = targetState;
	device.plugin.deck.setState(targetState, context);
}

void Button_Volume::onPressed() {
	/// Ignore first 300 ms
	repeatSkip = 3;
	trigger();
	repeatTimer.start();
}

void Button_Volume::onReleased() {
	repeatTimer.stop();
	state = -1;
	update();
}

void Button_Volume::trigger() {
	VoiceState *s = voiceState();
	if(!s)
		return;

	const int newVolume = static_cast<int>(qRound(static_cast<float>(qBound(minVolume, s->volume + step, maxVolume)) / step) * step);
	if(newVolume == s->volume)
		return;

	s->volume = newVolume;
	s->muted = false;
	state = -1;

	device.plugin.discord.sendCommand("SET_USER_VOICE_SETTINGS", {
		{"user_id", s->userID},
		{"volume",  QDiscord::uiToIPCVolume(newVolume)},
		{"mute",    false}
	});
	device.plugin.updateButtons();
}
