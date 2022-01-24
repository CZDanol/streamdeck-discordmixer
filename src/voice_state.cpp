#include "voice_state.h"

double VoiceState::ipcToUIVolume(double v) {
	if(v <= 100)
		return 17.361 * log(v) + 20.004;
	else
		return 144.63 * log(v) - 565.99;
}

double VoiceState::uiToIPCVolume(double v) {
	if(v <= 100)
		return exp((v - 20.004) / 17.361);
	else
		return exp((v + 565.99) / 144.63);
}

VoiceState VoiceState::fromJson(const QJsonObject &json) {
	VoiceState r;
	r.nick = json["nick"].toString();
	r.userID = json["user"]["id"].toString();
	r.volume = static_cast<int>(qRound(ipcToUIVolume(json["volume"].toDouble())));
	r.muted = json["mute"].toBool();
	return r;
}
