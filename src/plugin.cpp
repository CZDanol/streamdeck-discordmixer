#include "plugin.h"

#include <ESDConnectionManager.h>

int c = 0;

Plugin::Plugin() {
}

void Plugin::KeyDownForAction(const std::string &inAction, const std::string &inContext, const json &inPayload, const std::string &inDeviceID) {
	action_ = inAction;
	context_ = inContext;
	payload_ = inPayload;
	device_ = inDeviceID;

	//mConnectionManager->SetTitle(to_string(c++), inContext, kESDSDKTarget_HardwareAndSoftware);

	if(action_ == "cz.danol.discordmixer.openmixer")
		mConnectionManager->SwitchToProfile(device_, "Discord Volume Mixer");

	else if(action_ == "cz.danol.discordmixer.closemixer")
		mConnectionManager->SwitchToProfile(device_, "");

	else if(action_ == "cz.danol.discordmixer.user")
		return;
}
