#pragma once

#include <QString>
#include <QJsonArray>

#include <qtstreamdeck/qstreamdeckaction.h>

#include "voice_state.h"

class Plugin;
class Button;

class Device : public QObject {
	Q_OBJECT

public:
	using UserIx = int;

public:
	Device(Plugin &plugin, const QString &deviceID, const QJsonObject &deviceInfo);
	~Device();

public:
	void onAppear(const QStreamDeckAction &action);
	void onDisappear(const QStreamDeckAction &action);
	void onSettingsReceived(const QStreamDeckAction &action);
	void onDiscordMessage(const QJsonObject &msg);

public:
	void updateData();
	Q_SLOT void updateButtons();

private:
	void loadSelfVoiceStateUpdate(const QJsonObject &json);

public:
	Plugin &plugin;
	const QString deviceID;
	const QJsonObject deviceInfo;

public:
	QHash<QString, Button*> buttons;
	QMap<QString, VoiceState> voiceStates;
	QSet<QString> speakingUsers;

	/// Used in pagination
	int userIxOffset = 0;

	int nextPageButtons = 0;
	int prevPageButtons = 0;

	bool microphoneMuted = false;
	bool deafened = false;

};

