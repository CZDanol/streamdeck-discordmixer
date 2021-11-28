#include "device.h"

#include "actiontype.h"
#include "plugin.h"

Device::Device(Plugin &plugin, const QString &deviceID) : plugin_(plugin), deviceID_(deviceID) {

}

void Device::onAppear(const QStreamDeckAction &action) {
	if(action.action == ActionType::user) {
		qDebug() << "APPEAR" << action.payload;
		const UserIx userIx = action.payload["settings"]["user_ix"].toString().toInt();
		userButtons_.insert(userIx, action.context);
		updateUserButton(userIx, action.context);
	}
}

void Device::onDisappear(const QStreamDeckAction &action) {
	if(action.action == ActionType::user) {
		qDebug() << "DISAPPEAR" << action.payload;
		userButtons_.remove(action.payload["settings"]["user_ix"].toString().toInt(), action.context);
	}
}

void Device::updateAll() {
	const QJsonObject voiceChannelData = plugin_.discord.sendCommandAndGetResponse("GET_SELECTED_VOICE_CHANNEL", {});

	voiceStates_.clear();
	for(const auto &v: voiceChannelData["data"]["voice_states"].toArray())
		voiceStates_ += v.toObject();

	for(auto it = userButtons_.begin(), end = userButtons_.end(); it != end; it++)
		updateUserButton(it.key(), it.value());
}

void Device::updateUserButton(UserIx userIx, const ActionContext &ctx) {
	const QJsonObject json = voiceStates_.value(userIx + userIxOffset_);

	plugin_.deck.setTitle(QStringLiteral("%1\n\n%2 %").arg(json["nick"].toString(), json["volume"].toInt()), ctx, kESDSDKTarget_HardwareAndSoftware);

	qDebug() << "Update" << userIx << json;
}
