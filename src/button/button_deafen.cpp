#include "button_deafen.h"

Button_Deafen::Button_Deafen(const Button::CtorData &d) : Button(d) {
	device.specialButtons += this;
}

Button_Deafen::~Button_Deafen() {
	device.specialButtons -= this;
}

void Button_Deafen::onReleased() {
	device.deafened ^= 1;
	device.microphoneMuted |= device.deafened;
	device.plugin.discord.sendCommand("SET_VOICE_SETTINGS", {{"deaf", device.deafened}});
	device.updateSpecialButtons();
}

void Button_Deafen::update() {
	const int targetState = device.deafened;
	if(targetState == state)
		return;

	state = targetState;
	device.plugin.deck.setState(targetState, context);
}
