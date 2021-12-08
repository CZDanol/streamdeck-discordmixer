#include "button_user.h"

Button_User::Button_User(const Button::CtorData &d) : Button_UserRelated(d) {

}

void Button_User::update() {
	const VoiceState state = device.voiceStates.value(device.voiceStates.keys().value(effectiveIx()));
	const bool is = !state.nick.isEmpty();

	const QString volumeStr = state.muted ? "MUTED" : QStringLiteral("%1 %").arg(QString::number(state.volume));
	const QString newTitle = is ? QStringLiteral("%1\n\n%2").arg(state.nick, volumeStr) : QString();

	if(title == newTitle)
		return;

	title = newTitle;
	device.plugin.deck.setTitle(newTitle, context, kESDSDKTarget_HardwareAndSoftware);
}

void Button_User::onPressed() {
	const QString id = device.voiceStates.keys().value(effectiveIx());
	if(id.isEmpty())
		return;

	VoiceState &state = device.voiceStates[id];

	state.muted = !state.muted;

	device.plugin.discord.sendCommand("SET_USER_VOICE_SETTINGS", {
		{"user_id", state.userID},
		{"mute",  state.muted}
	});
	device.updateUserRelatedButtons(userIx);
}
