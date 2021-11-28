#pragma once

#include <QString>
#include <QJsonObject>

struct VoiceState {

public:
	static VoiceState fromJson(const QJsonObject &json);

public:
	QString nick;
	QString userID;
	int volume;

};

