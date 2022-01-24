#include "button_user.h"

#include <QBuffer>
#include <QPainter>

Button_User::Button_User(const Button::CtorData &d) : Button_UserRelated(d) {

}

void Button_User::update() {
	const VoiceState vs = device.voiceStates.value(device.voiceStates.keys().value(effectiveIx()));
	const bool is = !vs.nick.isEmpty();

	const QString volumeStr = vs.muted ? "####MUTED####" : QStringLiteral("%1 %").arg(QString::number(vs.volume));

	QString newTitle;
	if(is)
		newTitle = QStringLiteral("%1\n%3\n%2").arg(vs.nick, volumeStr, vs.speaking ? ">>SPEAKING<<" : vs.muted ? "##" : "");
	else if(device.voiceStates.isEmpty())
		newTitle = QString("NOBODY\nIN\nVOICE CHAT");

	if(title != newTitle) {
		title = newTitle;
		device.plugin.deck.setTitle(newTitle, context, kESDSDKTarget_HardwareAndSoftware);
	}

	QString newUserId = vs.userID;
	if(userId != newUserId || !hasAvatar) {
		userId = newUserId;

		const QImage avatar = device.plugin.discord.getUserAvatar(userId, vs.avatarID).scaled(72, 72, Qt::KeepAspectRatio, Qt::SmoothTransformation);
		hasAvatar = !avatar.isNull();

		QImage img(72, 72, QImage::Format_ARGB32);
		const auto setImage = [&] (int state) {
			QByteArray ba;
			QBuffer buf(&ba);
			buf.open(QIODevice::WriteOnly);
			img.save(&buf, "PNG");
			device.plugin.deck.setImage(ba.toBase64(), context, state);
		};

		QPainter p(&img);
		img.fill(Qt::transparent);
		p.setOpacity(0.5);
		p.drawImage(0, 0, avatar);
		p.setOpacity(1);
		setImage(0);

		static const QImage speakingDeco("icons/speaking_deco.png");
		p.drawImage(0, 0, speakingDeco);
		setImage(1);
	}

	const int newState = vs.speaking ? 1 : 0;
	if(state != newState) {
		state = newState;
		device.plugin.deck.setState(state, context);
	}
}

void Button_User::onPressed() {
	const QString id = device.voiceStates.keys().value(effectiveIx());
	if(id.isEmpty())
		return;

	VoiceState &vs = device.voiceStates[id];

	vs.muted = !vs.muted;

	device.plugin.discord.sendCommand("SET_USER_VOICE_SETTINGS", {
		{"user_id", vs.userID},
		{"mute",    vs.muted}
	});
	device.plugin.deck.setState(state, context);
	device.updateButtons();
}

void Button_User::onReleased() {
	device.plugin.deck.setState(state, context);
}
