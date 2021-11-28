#include "button_volume.h"

static constexpr int minVolume = 0;
static constexpr int maxVolume = 140;

Button_Volume::Button_Volume(const Button::CtorData &d) : Button_UserRelated(d) {
	step = d.action == "cz.danol.discordmixer.volumeup" ? 15 : -15;
}

void Button_Volume::onPressed() {
	VoiceState *s = voiceState();
	if(!s)
		return;

	const int newVolume = qBound(minVolume, s->volume + step, maxVolume);

	if(newVolume == s->volume)
		return;

	s->volume = newVolume;
	state = -1;
	device.plugin.discord.sendCommand("SET_USER_VOICE_SETTINGS", {
		{"user_id", s->userID},
		{"volume",  newVolume}
	});
	device.updateUserRelatedButtons(userIx);
}

void Button_Volume::update() {
	VoiceState *s = voiceState();
	if(!s)
		return;

	const int targetState = (step < 0 && s->volume <= minVolume) || (step > 0 && s->volume >= maxVolume);
	if(state == targetState)
		return;

	state = targetState;
	device.plugin.deck.setState(targetState, context);
}

VoiceState *Button_Volume::voiceState() {
	const int ix = effectiveIx();
	if(ix >= device.voiceStates.size())
		return nullptr;

	return &device.voiceStates[ix];
}
