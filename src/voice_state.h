#pragma once

#include <QString>
#include <QJsonObject>

struct VoiceState {

public:
	static VoiceState fromJson(const QJsonObject &json);

public:
	QString nick;
	QString userID;
	int volume = 0;
	bool muted = false;
	bool speaking = false;

};

