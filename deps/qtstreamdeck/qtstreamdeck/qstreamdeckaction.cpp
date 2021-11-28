#include "qstreamdeckaction.h"

bool QStreamDeckAction::fromJson(const QJsonObject &json) {
	this->action = json["action"].toString();
	this->deviceId = json["device"].toString();
	this->event = json["event"].toString();
	this->context = json["context"].toString();
	this->payload = json["payload"].toObject();
	return true;
}
