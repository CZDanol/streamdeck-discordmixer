#include "qstreamdeckplugin.h"

QStreamDeckPlugin::QStreamDeckPlugin(QObject *parent) : QObject(parent) {
}

QStreamDeckPlugin::~QStreamDeckPlugin() {
	disconnect();
}

void QStreamDeckPlugin::init(int port, const QString &pluginUUID, const QString &registerEvent, const QString &info) {
	m_plugin = ESDPluginModel(port, pluginUUID, registerEvent, info);
}

bool QStreamDeckPlugin::connect() {
	// Validate command line arguments
	if(!this->m_plugin.validateParameters()) {
		return false;
	}

	m_websocket.open("ws://localhost:" + QString::number(m_plugin.port));

	if(m_websocket.isValid()) {
		return false;
	}

	QObject::connect(&m_websocket, &QWebSocket::connected, this, &QStreamDeckPlugin::streamdeck_onConnected);
	QObject::connect(&m_websocket, &QWebSocket::disconnected, this, &QStreamDeckPlugin::streamdeck_onDisconnected);

	return true;
}

bool QStreamDeckPlugin::disconnect() {
	if(m_websocket.isValid()) {
#ifdef DEBUG
		qDebug()<<"ESDPluginBase => Communication is closed!";
#endif
		QObject::disconnect(&m_websocket, &QWebSocket::textMessageReceived, this, &QStreamDeckPlugin::streamdeck_onDataReceived);
		QObject::disconnect(&m_websocket, &QWebSocket::connected, this, &QStreamDeckPlugin::streamdeck_onConnected);
		QObject::disconnect(&m_websocket, &QWebSocket::disconnected, this, &QStreamDeckPlugin::streamdeck_onDisconnected);

		m_websocket.close(QWebSocketProtocol::CloseCodeNormal, "Normal exit");
		return true;
	}

	return false;
}

bool QStreamDeckPlugin::isDeviceConnected() {
	return m_deviceConnected;
}

// ------ COMMUNICATION --------

void QStreamDeckPlugin::writeJSON(QJsonObject &jsonObject) {
	QJsonDocument doc(jsonObject);
	QString jsonStr(doc.toJson(QJsonDocument::Compact));

#ifdef DEBUG
	qInfo()<<"ESDPluginBase => WriteJSON: "<<jsonStr;
#endif

	m_websocket.sendTextMessage(jsonStr);
}

void QStreamDeckPlugin::streamdeck_onConnected() {
#ifdef DEBUG
	qDebug()<<"ESDPluginBase => WebSocket is open!";
#endif

	QObject::connect(&m_websocket, &QWebSocket::textMessageReceived, this, &QStreamDeckPlugin::streamdeck_onDataReceived);

	// Register plugin
	QJsonObject pluginRegisterData;
	m_plugin.toJson(pluginRegisterData);
	writeJSON(pluginRegisterData);
}

void QStreamDeckPlugin::streamdeck_onDisconnected() {
#ifdef DEBUG
	qDebug()<<"ESDPluginBase => Websocket is closed! Close reason: "<<m_websocket.closeReason();
#endif
	QObject::disconnect(&m_websocket, &QWebSocket::textMessageReceived, this, &QStreamDeckPlugin::streamdeck_onDataReceived);
	QObject::disconnect(&m_websocket, &QWebSocket::connected, this, &QStreamDeckPlugin::streamdeck_onConnected);
	QObject::disconnect(&m_websocket, &QWebSocket::disconnected, this, &QStreamDeckPlugin::streamdeck_onDisconnected);
}

void QStreamDeckPlugin::streamdeck_onDataReceived(QString message) {
#ifdef DEBUG
	qDebug()<<"ESDPluginBase => Received JSON: "<<message;
#endif

	QJsonDocument jsonMessage = QJsonDocument::fromJson(message.toUtf8());
	QJsonObject jsonMessagObject = jsonMessage.object();

	bool messageOk = parseMessage(jsonMessagObject);

#ifdef DEBUG
	if(!messageOk)
	{
			qDebug()<<"ESDPluginBase => Could not parse received message!";
	}
#endif
}

bool QStreamDeckPlugin::parseMessage(QJsonObject &jsonObject) {
	// Device info message
	if(jsonObject.contains("device")) {
		ESDDeviceModel deviceModel;
		deviceModel.fromJson(jsonObject);

		if(deviceModel.event == kESDSDKEventDeviceDidConnect) {
			this->m_deviceConnected = true;
			emit deviceDidConnect(deviceModel.deviceId, deviceModel.deviceInfo);
			return true;
		}
		else if(deviceModel.event == kESDSDKEventDeviceDidDisconnect) {
			this->m_deviceConnected = false;
			emit deviceDidDisconnect(deviceModel.deviceId);
			return true;
		}
	}

	// Action message
	if(jsonObject.contains("action")) {
		QStreamDeckAction action;
		action.fromJson(jsonObject);

		static const QHash<QString, void (QStreamDeckPlugin::*)(const QStreamDeckAction &)> actionFuncs{
			{kESDSDKEventKeyDown,                       &QStreamDeckPlugin::keyDown},
			{kESDSDKEventKeyUp,                         &QStreamDeckPlugin::keyUp},
			{kESDSDKEventWillAppear,                    &QStreamDeckPlugin::willAppear},
			{kESDSDKEventWillDisappear,                 &QStreamDeckPlugin::willDisappear},
			{kESDSDKEventTitleParametersDidChange,      &QStreamDeckPlugin::titleParametersDidChange},
			{kESDSDKEventPropertyInspectorDidAppear,    &QStreamDeckPlugin::propertyInspectorDidAppear},
			{kESDSDKEventPropertyInspectorDidDisappear, &QStreamDeckPlugin::propertyInspectorDidDisappear},
			{kESDSDKEventSendToPlugin,                  &QStreamDeckPlugin::sendToPlugin},
			{kESDSDKEventDidReceiveSettings,            &QStreamDeckPlugin::didReceiveSettings},
			{kESDSDKEventDidReceiveGlobalSettings,      &QStreamDeckPlugin::didReceiveGlobalSettings},
		};

		if(auto func = actionFuncs.value(action.event)) {
			(this->*func)(action);
			return true;
		}
	}

		// Event message
	else if(jsonObject.contains("event")) {
		ESDEventModel eventMsg;
		eventMsg.fromJson(jsonObject);

		// ApplicationDidLaunch event
		if(eventMsg.event == kESDSDKEventApplicationDidLaunch) {
			emit applicationDidLaunch(eventMsg.event, eventMsg.payload);
			return true;
		}

		// ApplicationDidTerminate event
		if(eventMsg.event == kESDSDKEventApplicationDidTerminate) {
			emit applicationDidTerminate(eventMsg.event, eventMsg.payload);
			return true;
		}

		// SystemDidWakeUp
		if(eventMsg.event == kESDSDKEventSystemDidWakeUp) {
			emit systemDidWakeUp();
			return true;
		}
	}

	return false;
}

// ------ SLOTS --------

void QStreamDeckPlugin::logMessage(const QString &message) {
	QJsonObject jsonObject;

	jsonObject[kESDSDKCommonEvent] = kESDSDKEventLogMessage;

	QJsonObject payload;
	payload[kESDSDKPayloadMessage] = message;
	jsonObject[kESDSDKCommonPayload] = payload;

	writeJSON(jsonObject);
}

void QStreamDeckPlugin::cleanup() {
	this->disconnect();
}

void QStreamDeckPlugin::switchProfile(const QString &deviceId, const QString &profileName) {
	QJsonObject jsonObject;

	jsonObject[kESDSDKCommonEvent] = kESDSDKEventSwitchToProfile;
	jsonObject[kESDSDKCommonContext] = m_plugin.pluginUUID;
	jsonObject[kESDSDKCommonDevice] = deviceId;

	QJsonObject payload;
	payload[kESDSDKPayloadProfile] = profileName;
	jsonObject[kESDSDKCommonPayload] = payload;

	writeJSON(jsonObject);
}

void QStreamDeckPlugin::sendToPropertyInspector(const QString &action, const QString &context, QJsonObject &payload) {
	QJsonObject jsonObject;

	jsonObject[kESDSDKCommonEvent] = kESDSDKEventSendToPropertyInspector;
	jsonObject[kESDSDKCommonContext] = context;
	jsonObject[kESDSDKCommonAction] = action;
	jsonObject[kESDSDKCommonPayload] = payload;

	writeJSON(jsonObject);
}

void QStreamDeckPlugin::setState(int state, const QString &context) {
	QJsonObject jsonObject;

	QJsonObject payload;
	payload[kESDSDKPayloadState] = state;

	jsonObject[kESDSDKCommonEvent] = kESDSDKEventSetState;
	jsonObject[kESDSDKCommonContext] = context;
	jsonObject[kESDSDKCommonPayload] = payload;

	writeJSON(jsonObject);
}

void QStreamDeckPlugin::setTitle(const QString &title, const QString &context, ESDSDKTarget target) {
	QJsonObject jsonObject;

	jsonObject[kESDSDKCommonEvent] = kESDSDKEventSetTitle;
	jsonObject[kESDSDKCommonContext] = context;

	QJsonObject payload;
	payload[kESDSDKPayloadTarget] = target;
	payload[kESDSDKPayloadTitle] = title;
	jsonObject[kESDSDKCommonPayload] = payload;

	writeJSON(jsonObject);
}

void QStreamDeckPlugin::setImage(const QString &base64Str, const QString &context, ESDSDKTarget target) {
	QJsonObject jsonObject;
	const QString prefix = "data:image/png;base64,";

	jsonObject[kESDSDKCommonEvent] = kESDSDKEventSetImage;
	jsonObject[kESDSDKCommonContext] = context;

	QJsonObject payload;
	payload[kESDSDKPayloadTarget] = target;

	if(base64Str.contains(prefix)) {
		payload[kESDSDKPayloadImage] = base64Str;
	}
	else {
		payload[kESDSDKPayloadImage] = prefix + base64Str;
	}

	jsonObject[kESDSDKCommonPayload] = payload;
	writeJSON(jsonObject);
}

void QStreamDeckPlugin::showAlert(const QString &context) {
	QJsonObject jsonObject;

	jsonObject[kESDSDKCommonEvent] = kESDSDKEventShowAlert;
	jsonObject[kESDSDKCommonContext] = context;

	writeJSON(jsonObject);
}

void QStreamDeckPlugin::showOk(const QString &context) {
	QJsonObject jsonObject;

	jsonObject[kESDSDKCommonEvent] = kESDSDKEventShowOK;
	jsonObject[kESDSDKCommonContext] = context;

	writeJSON(jsonObject);
}

void QStreamDeckPlugin::setSettings(QJsonObject &settings, const QString &context) {
	QJsonObject jsonObject;

	jsonObject[kESDSDKCommonEvent] = kESDSDKEventSetSettings;
	jsonObject[kESDSDKCommonContext] = context;
	jsonObject[kESDSDKCommonPayload] = settings;

	writeJSON(jsonObject);
}

void QStreamDeckPlugin::setGlobalSettings(QJsonObject &settings, const QString &context) {
	QJsonObject jsonObject;

	jsonObject[kESDSDKCommonEvent] = kESDSDKEventSetGlobalSettings;
	jsonObject[kESDSDKCommonContext] = context;
	jsonObject[kESDSDKCommonPayload] = settings;

	writeJSON(jsonObject);
}

void QStreamDeckPlugin::getSettings(const QString &context) {
	QJsonObject jsonObject;
	jsonObject[kESDSDKCommonEvent] = kESDSDKEventGetSettings;
	jsonObject[kESDSDKCommonContext] = context;

	writeJSON(jsonObject);
}

void QStreamDeckPlugin::getGlobalSettings(QJsonObject &context) {
	QJsonObject jsonObject;
	jsonObject[kESDSDKCommonEvent] = kESDSDKEventGetGlobalSettings;
	jsonObject[kESDSDKCommonContext] = context;

	writeJSON(jsonObject);
}

void QStreamDeckPlugin::openURL(const QString &url) {
	QJsonObject jsonObject;
	QJsonObject payload;

	payload["url"] = url;

	jsonObject[kESDSDKCommonEvent] = kESDSDKEventOpenURL;
	jsonObject[kESDSDKCommonPayload] = payload;

	writeJSON(jsonObject);
}

