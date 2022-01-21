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
	Device(Plugin &plugin, const QString &deviceID, const QJsonObject &deviceInfo);
	~Device();

public:
	void onAppear(const QStreamDeckAction &action);
	void onDisappear(const QStreamDeckAction &action);
	void onSendToPlugin(const QStreamDeckAction &action);
	void onDiscordMessage(const QJsonObject &msg);

public:
	void updateData();
	void updateAllButtons();
	void updateUserRelatedButtons(UserIx userIx);
	void updateSpecialButtons();

private:
	void loadSelfVoiceStateUpdate(const QJsonObject &json);

public:
	Plugin &plugin;
	const QString deviceID;
	const QJsonObject deviceInfo;

public:
	QHash<QString, Button*> buttons;
	QMultiHash<UserIx, Button*> userRelatedButtons;
	QSet<Button*> specialButtons;
	QMap<QString, VoiceState> voiceStates;

	/// Used in pagination
	int userIxOffset = 0;

	bool microphoneMuted = false;
	bool deafened = false;

};

