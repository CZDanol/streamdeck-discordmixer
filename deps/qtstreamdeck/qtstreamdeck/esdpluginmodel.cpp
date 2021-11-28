#include "esdpluginmodel.h"
#include <QDebug>

ESDPluginModel::ESDPluginModel(int port, QString pluginUUID, QString registerEvent, QString info) {
	this->port = port;
	this->pluginUUID = pluginUUID;
	this->registerEvent = registerEvent;
	this->info = info;
}

bool ESDPluginModel::validateParameters() {
	// Check websocket port
	if(this->port <= 0) {
#ifdef DEBUG
		qDebug()<<"ESDPluginModel => ERROR: Websocket port can't be less than or equal 0!";
#endif
		return false;
	}

	// Check pluginUUID
	if(this->pluginUUID.isEmpty()) {
#ifdef DEBUG
		qDebug()<<"ESDPluginModel => ERROR: PluginUUID can't be empty!";
#endif
		return false;
	}

	// Check registerEvent
	if(this->registerEvent.isEmpty()) {
#ifdef DEBUG
		qDebug()<<"ESDPluginModel => ERROR: RegisterEvent can't be empty!";
#endif
		return false;
	}

	// Check info
	if(this->info.isEmpty()) {
#ifdef DEBUG
		qDebug()<<"ESDPluginModel => ERROR: Info can't be empty!";
#endif
		return false;
	}

	return true;
}

bool ESDPluginModel::fromJson(const QJsonObject &json) {
	QStringList keyList = {"port", "uuid", "event", "info"};
	if(!IDataModel::containsKeys(json, keyList)) {
#ifdef DEBUG
		qDebug()<<"ESDPluginModel => JSON does not contain valid keys!";
#endif
		return false;
	}

	this->port = json["port"].toInt();
	this->pluginUUID = json["uuid"].toString();
	this->registerEvent = json["event"].toString();
	this->info = json["info"].toString();

	return true;
}

bool ESDPluginModel::toJson(QJsonObject &json) {
	json["uuid"] = this->pluginUUID;
	json["event"] = this->registerEvent;
	return true;
}
