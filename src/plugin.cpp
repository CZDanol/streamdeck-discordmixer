#include "plugin.h"

#include <ESDConnectionManager.h>

void Plugin::KeyDownForAction(const std::string &inAction, const std::string &inContext, const json &inPayload, const std::string &inDeviceID) {
	action_ = inAction;
	context_ = inContext;
	payload_ = inPayload;
	device_ = inDeviceID;

	if(action_ == "cz.danol.discordmixer.openMixer")
		mConnectionManager->SwitchToProfile(device_, "Discord Volume Mixer");

	else if(action_ == "cz.danol.discordmixer.closeMixer")
		mConnectionManager->SwitchToProfile(device_, nullptr);

	else if(action_ == "cz.danol.discordmixer.user")
		return;
}
