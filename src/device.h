#pragma once

#include <QString>
#include <QJsonArray>

#include <qtstreamdeck/qstreamdeckaction.h>

#include "actiontype.h"

class Plugin;
class Button;

class Device {

public:
	Device(Plugin &plugin, const QString &deviceID);
	~Device();

public:
	void onAppear(const QStreamDeckAction &action);
	void onDisappear(const QStreamDeckAction &action);

public:
	void updateData();
	void updateAllButtons();

public:
	Plugin &plugin;
	const QString deviceID;

public:
	QHash<QString, Button*> buttons;
	QMultiHash<UserIx, Button*> userRelatedButtons;

	/// Array of all users in the voice channel data
	QList<QJsonObject> voiceStates;

	/// Used in pagination
	int userIxOffset = 0;

};

