#include "voice_state.h"

VoiceState VoiceState::fromJson(const QJsonObject &json) {
	VoiceState r;
	r.nick = json["nick"].toString();
	r.userID = json["user"]["id"].toString();
	r.volume = json["volume"].toInt();
	return r;
}
