#include "plugin.h"

#include <ESDConnectionManager.h>

struct ButtonType {
	static const inline string openMixer = "cz.danol.discordmixer.openmixer";
	static const inline string closeMixer = "cz.danol.discordmixer.closemixer";
	static const inline string user = "cz.danol.discordmixer.user";
};

int crr = 0;

Plugin::Plugin() {
	{
		discord::Core *core = nullptr;
		crr = static_cast<int>(discord::Core::Create(914314199436509185, static_cast<uint64_t>(discord::CreateFlags::Default), &core));
		discord_ = unique_ptr<discord::Core>(core);
	}
}

void Plugin::KeyDownForAction(const std::string &inAction, const std::string &inContext, const json &inPayload, const std::string &inDeviceID) {
	//mConnectionManager->SetTitle(to_string(c++), inContext, kESDSDKTarget_HardwareAndSoftware);

	if(inAction == ButtonType::openMixer)
		mConnectionManager->SwitchToProfile(inDeviceID, "Discord Volume Mixer");

	else if(inAction == ButtonType::closeMixer)
		mConnectionManager->SwitchToProfile(inDeviceID, "");

	else if(inAction == ButtonType::user) {
		updateUserButton(inContext);
		return;
	}
}

void Plugin::WillAppearForAction(const string &inAction, const string &inContext, const json &inPayload, const string &inDeviceID) {
	{
		KeyData d;
		d.x = inPayload["coordinates"]["column"].get<int>() - 1;
		d.y = inPayload["coordinates"]["row"].get<int>() - 1;
		d.settings = inPayload["settings"];

		keyData_[inContext] = d;
	}

	if(inAction == ButtonType::user) {
		updateUserButton(inContext);
	}
}

void Plugin::WillDisappearForAction(const string &inAction, const string &inContext, const json &inPayload, const string &inDeviceID) {
	keyData_.erase(inContext);
}

void Plugin::updateUserButton(const string &context) {
	KeyData &kd = keyData_[context];
	auto &lm = discord_->LobbyManager();

	discord::LobbyId lobbyId = 0;
	lm.GetLobbyId(0, &lobbyId);

	discord::UserId userId = 0;
	lm.GetMemberUserId(lobbyId, kd.settings["user_ix"].get<int>() + usersIxOffset_, &userId);

	discord::User user;
	lm.GetMemberUser(lobbyId, userId, &user);

	mConnectionManager->SetTitle("U" + user.GetUsername(), context, kESDSDKTarget_HardwareAndSoftware);
}
