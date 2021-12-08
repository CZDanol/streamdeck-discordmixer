#include "voice_state.h"

VoiceState VoiceState::fromJson(const QJsonObject &json) {
	VoiceState r;
	r.nick = json["nick"].toString();
	r.userID = json["user"]["id"].toString();
	r.volume = static_cast<int>(json["volume"].toDouble());
	r.muted = json["mute"].toBool();
	return r;
}
