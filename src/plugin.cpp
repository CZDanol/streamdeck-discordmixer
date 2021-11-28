#include "plugin.h"

#include <ESDConnectionManager.h>

struct ButtonType {
	static const inline string openMixer = "cz.danol.discordmixer.openmixer";
	static const inline string closeMixer = "cz.danol.discordmixer.closemixer";
	static const inline string user = "cz.danol.discordmixer.user";
};

Plugin::Plugin() {
	log_.open("log.txt", ios_base::app);

	{
		discord::Core *core = nullptr;
		auto r = static_cast<int>(discord::Core::Create(914314199436509185, static_cast<uint64_t>(discord::CreateFlags::Default), &core));
		discord_ = unique_ptr<discord::Core>(core);
		log_ << "DAPPP " << r << endl;
	}

	{
		discord::Activity a;
		a.SetName("TESTING");
		discord_->ActivityManager().UpdateActivity(a, [](const auto &) {});
	}

	discord_->RunCallbacks();

	discord_->OverlayManager().OpenVoiceSettings([](auto) {});

	{
		auto &rm = discord_->RelationshipManager();
		int32_t cnt = 0;
		rm.Count(&cnt);
		log_ << "REL CNT " << cnt << endl;

		for(int i = 0; i < cnt; i++) {
			discord::Relationship rel;
			rm.GetAt(i, &rel);

			log_ << "REL " << rel.GetUser().GetUsername() << endl;
		}
	}
}

void Plugin::KeyDownForAction(const std::string &inAction, const std::string &inContext, const json &inPayload, const std::string &inDeviceID) {
	discord_->RunCallbacks();

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
	discord_->RunCallbacks();

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

	mConnectionManager->SetTitle(to_string(lobbyId) /*+ user.GetUsername()*/, context, kESDSDKTarget_HardwareAndSoftware);
	return;

	discord::UserId userId = 0;
	lm.GetMemberUserId(lobbyId, kd.settings["user_ix"].get<int>() + usersIxOffset_, &userId);

	discord::User user;
	lm.GetMemberUser(lobbyId, userId, &user);
}
