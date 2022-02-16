#include "button_open_mixer.h"

Button_OpenMixer::Button_OpenMixer(const Button::CtorData &d) : Button(d) {

}

void Button_OpenMixer::onPressed() {
	static const QMap<int, QString> profileNameByDeviceType {
		{0, "Discord Volume Mixer"},
		{1, "Discord Volume Mixer Mini"},
		{2, "Discord Volume Mixer XL"},
		{3, "Discord Volume Mixer"}, // Mobile
	};

	device.plugin.deck.switchProfile(device.deviceID, profileNameByDeviceType.value(device.deviceInfo["type"].toInt(), "Discord Volume Mixer"));
	device.updateData();
}
