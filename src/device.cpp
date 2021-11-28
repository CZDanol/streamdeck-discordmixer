#include "device.h"

#include "actiontype.h"
#include "plugin.h"

Device::Device(Plugin &plugin, const QString &deviceID) : plugin_(plugin), deviceID_(deviceID) {

}

void Device::onAppear(const QStreamDeckAction &action) {
	if(action.action == ActionType::user) {
		qDebug() << "APPEAR" << action.payload;
		userActions_.insert(action.payload["user_ix"].toInt(), action.context);
	}
}

void Device::onDisappear(const QStreamDeckAction &action) {
	if(action.action == ActionType::user) {
		qDebug() << "DISAPPEAR" << action.payload;
		userActions_.remove(action.payload["user_ix"].toInt(), action.context);
	}
}

void Device::updateAll() {
	const QJsonObject voiceChannelData = plugin_.discord.sendCommandAndGetResponse("GET_SELECTED_VOICE_CHANNEL", {});
	voiceStates_ = voiceChannelData["data"]["voice_states"];
}

void Device::updateUserButton(UserIx userIx, const ActionContext &ctx) {

}
