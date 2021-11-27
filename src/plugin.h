#pragma once

#include <ESDBasePlugin.h>

class Plugin : public ESDBasePlugin {

public:
	virtual void KeyDownForAction(const std::string &inAction, const std::string &inContext, const json &inPayload, const std::string &inDeviceID) override;
	virtual void KeyUpForAction(const std::string &inAction, const std::string &inContext, const json &inPayload, const std::string &inDeviceID) override;
	virtual void WillAppearForAction(const std::string &inAction, const std::string &inContext, const json &inPayload, const std::string &inDeviceID) override;
	virtual void WillDisappearForAction(const std::string &inAction, const std::string &inContext, const json &inPayload, const std::string &inDeviceID) override;
	virtual void DeviceDidConnect(const std::string &inDeviceID, const json &inDeviceInfo) override;
	virtual void DeviceDidDisconnect(const std::string &inDeviceID) override;
	virtual void SendToPlugin(const std::string &inAction, const std::string &inContext, const json &inPayload, const std::string &inDeviceID) override;

};

