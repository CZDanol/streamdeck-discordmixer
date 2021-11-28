#include "button_open_mixer.h"

Button_OpenMixer::Button_OpenMixer(const Button::CtorData &d) : Button(d) {

}

void Button_OpenMixer::onPressed() {
	if(!device.plugin.discord.isConnected())
		device.plugin.discord.connect(clientID, clientSecret);

	device.plugin.deck.switchProfile(device.deviceID, "Discord Volume Mixer");
	device.updateData();
}
