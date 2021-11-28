#pragma once

#include <QString>

#include <qtstreamdeck/qstreamdeckaction.h>

#include "actiontype.h"

class Plugin;

class Device {

public:
	Device(Plugin &plugin, const QString &deviceID);

public:
	void onAppear(const QStreamDeckAction &action);
	void onDisappear(const QStreamDeckAction &action);

public:
	void updateAll();

private:
	void updateUserButton(UserIx userIx, const ActionContext &ctx);

private:
	Plugin &plugin_;
	const QString deviceID_;

private:
	QMultiHash<UserIx, ActionContext> userActions_;
	QJsonArray voiceStates_;

};

