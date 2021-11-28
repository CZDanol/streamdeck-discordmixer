#include "button_open_mixer.h"

Button_OpenMixer::Button_OpenMixer(const Button::CtorData &d) : Button(d) {
	clientID = d.payload["settings"]["client_id"].toString();
	clientSecret = d.payload["settings"]["client_secret"].toString();
}

void Button_OpenMixer::onPressed() {
	if(!device.plugin.discord.isConnected())
		device.plugin.discord.connect(clientID, clientSecret);

	device.plugin.deck.switchProfile(device.deviceID, "Discord Volume Mixer");
	device.updateData();
}
