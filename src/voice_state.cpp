#include "voice_state.h"

#include <qtdiscordipc/qdiscord.h>

VoiceState VoiceState::fromJson(const QJsonObject &json) {
	VoiceState r;

	r.nick = json["nick"].toString();
	r.userID = json["user"]["id"].toString();
	r.avatarID = json["user"]["avatar"].toString();
	r.volume = static_cast<int>(qRound(QDiscord::ipcToUIVolume(json["volume"].toDouble())));
	r.muted = json["mute"].toBool();

	return r;
}
