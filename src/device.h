#pragma once

#include <QString>
#include <QJsonArray>

#include <qtstreamdeck/qstreamdeckaction.h>

#include "voice_state.h"

class Plugin;
class Button;

class Device {

public:
	using UserIx = int;

public:
	Device(Plugin &plugin, const QString &deviceID);
	~Device();

public:
	void onAppear(const QStreamDeckAction &action);
	void onDisappear(const QStreamDeckAction &action);
	void onSendToPlugin(const QStreamDeckAction &action);

public:
	void updateData();
	void updateAllButtons();
	void updateUserRelatedButtons(UserIx userIx);

public:
	Plugin &plugin;
	const QString deviceID;

public:
	QHash<QString, Button*> buttons;
	QMultiHash<UserIx, Button*> userRelatedButtons;
	QList<VoiceState> voiceStates;

	/// Used in pagination
	int userIxOffset = 0;

};

