#pragma once

#include <ESDBasePlugin.h>

using namespace std;

class Plugin : public ESDBasePlugin {

public:
	Plugin();

public:
	virtual void KeyDownForAction(const std::string &inAction, const std::string &inContext, const json &inPayload, const std::string &inDeviceID) override;

private:
	string action_, context_, device_;
	json payload_;

};

