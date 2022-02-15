#include "button_deafen.h"

void Button_Deafen::onReleased() {
	device.deafened ^= 1;
	device.microphoneMuted |= device.deafened;
	device.plugin.discord.sendCommand("SET_VOICE_SETTINGS", {{"deaf", device.deafened}});
	device.plugin.updateButtons();
}

void Button_Deafen::update() {
	const int targetState = device.deafened;
	if(targetState == state)
		return;

	state = targetState;
	device.plugin.deck.setState(targetState, context);
}
