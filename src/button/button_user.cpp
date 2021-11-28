#include "button_user.h"

Button_User::Button_User(const Button::CtorData &d) : Button_UserRelated(d) {

}

void Button_User::update() {
	const QJsonObject json = device.voiceStates.value(effectiveIx());

	const QString title = QStringLiteral("%1\n\n%2 %").arg(json["nick"].toString(), json["volume"].toInt());
	device.plugin.deck.setTitle(title, context, kESDSDKTarget_HardwareAndSoftware);
}
