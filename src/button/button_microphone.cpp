#include "button_microphone.h"

Button_Microphone::Button_Microphone(const Button::CtorData &d) : Button(d) {
	device.specialButtons += this;
}

Button_Microphone::~Button_Microphone() {
	device.specialButtons -= this;
}

void Button_Microphone::onReleased() {
	device.microphoneMuted ^= 1;
	device.plugin.discord.sendCommand("SET_VOICE_SETTINGS", {{"mute", device.microphoneMuted}});
	device.updateSpecialButtons();
}

void Button_Microphone::update() {
	const int targetState = device.microphoneMuted;
	if(targetState == state)
		return;

	state = targetState;
	device.plugin.deck.setState(targetState, context);
}
