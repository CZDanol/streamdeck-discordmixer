#pragma once

#include <ESDBasePlugin.h>
#include <discord.h>
#include <fstream>

using namespace std;

class Plugin : public ESDBasePlugin {

public:
	Plugin();

public:
	virtual void KeyDownForAction(const std::string &inAction, const std::string &inContext, const json &inPayload, const std::string &inDeviceID) override;

	virtual void WillAppearForAction(const string &inAction, const string &inContext, const json &inPayload, const string &inDeviceID) override;
	virtual void WillDisappearForAction(const string &inAction, const string &inContext, const json &inPayload, const string &inDeviceID) override;

private:
	void updateUserButton(const string &context);

private:
	unique_ptr<discord::Core> discord_;
	ofstream log_;

private:
	int usersIxOffset_ = 0;

private:
	struct KeyData {
		int x, y;
		json settings;
	};
	std::unordered_map<string, KeyData> keyData_;

};

