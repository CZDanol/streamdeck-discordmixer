#include "button_microphone.h"

void Button_Microphone::onReleased() {
	device.microphoneMuted ^= 1;
	device.plugin.discord.sendCommand("SET_VOICE_SETTINGS", {{"mute", device.microphoneMuted}});
	device.plugin.updateButtons();
}

void Button_Microphone::update() {
	const int targetState = device.microphoneMuted;
	if(targetState == state)
		return;

	state = targetState;
	device.plugin.deck.setState(targetState, context);
}
