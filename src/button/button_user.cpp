#include "button_user.h"

Button_User::Button_User(const Button::CtorData &d) : Button_UserRelated(d) {

}

void Button_User::update() {
	const VoiceState state = device.voiceStates.value(effectiveIx());
	const bool is = !state.nick.isEmpty();

	const QString title = is ? QStringLiteral("%1\n\n%2 %").arg(state.nick, QString::number(state.volume)) : QString();
	device.plugin.deck.setTitle(title, context, kESDSDKTarget_HardwareAndSoftware);
}
