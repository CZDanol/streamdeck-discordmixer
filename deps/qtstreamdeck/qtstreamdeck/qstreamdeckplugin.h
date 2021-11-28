#pragma once
//==============================================================================
/**
@file       ESDPlugin.h
@brief      Defines plugin source which communicates with Elgato Stream Deck application
@copyright  (c) 2019, Aleksandar Zdravkovic
            This source code is licensed under MIT licence
**/
//==============================================================================

#include <QObject>
#include <QtWebSockets/QWebSocket>
#include <QJsonDocument>
#include "qtstreamdeck/esddevicemodel.h"
#include "qtstreamdeck/qstreamdeckaction.h"
#include "qtstreamdeck/esdpluginmodel.h"
#include "qtstreamdeck/esdeventmodel.h"

#ifdef DEBUG
#include <QDebug>
#endif

class QStreamDeckPlugin : public QObject {
Q_OBJECT

public:
	QStreamDeckPlugin(QObject *parent = nullptr);
	~QStreamDeckPlugin();

public:
	void init(int port, const QString &pluginUUID, const QString &registerEvent, const QString &info);

	bool connect();
	bool disconnect();

	bool isDeviceConnected();

private:
	void writeJSON(QJsonObject &jsonObject);
	bool parseMessage(QJsonObject &jsonObject);

signals:
	void deviceDidConnect(const QString &deviceId, QJsonObject &deviceInfo);
	void deviceDidDisconnect(const QString &deviceId);

	void keyDown(const QStreamDeckAction &action);
	void keyUp(const QStreamDeckAction &action);
	void willAppear(const QStreamDeckAction &action);
	void willDisappear(const QStreamDeckAction &action);
	void titleParametersDidChange(const QStreamDeckAction &action);
	void applicationDidLaunch(const QString &event, QJsonObject &payload);
	void applicationDidTerminate(const QString &event, QJsonObject &payload);
	void propertyInspectorDidAppear(const QStreamDeckAction &action);
	void propertyInspectorDidDisappear(const QStreamDeckAction &action);
	void sendToPlugin(const QStreamDeckAction &action);
	void didReceiveSettings(const QStreamDeckAction &action);
	void didReceiveGlobalSettings(const QStreamDeckAction &action);
	void systemDidWakeUp();

public slots:
	void setSettings(QJsonObject &settings, const QString &context);
	void setGlobalSettings(QJsonObject &settings, const QString &context);
	void getSettings(const QString &context);
	void getGlobalSettings(QJsonObject &context);
	void openURL(const QString &url);
	void setState(int state, const QString &context);
	void setTitle(const QString &title, const QString &context, ESDSDKTarget target = 0);
	void setImage(const QString &base64Str, const QString &context, ESDSDKTarget target = 0);
	void showAlert(const QString &context);
	void showOk(const QString &context);
	void sendToPropertyInspector(const QString &action, const QString &context, QJsonObject &payload);
	void switchProfile(const QString &deviceId, const QString &profileName);
	void logMessage(const QString &message);
	void cleanup(); //For closing connection before exiting in application

private slots:
	void streamdeck_onConnected();
	void streamdeck_onDisconnected();
	void streamdeck_onDataReceived(QString message);

private:
	ESDPluginModel m_plugin;
	QWebSocket m_websocket;
	ESDSDKDeviceType m_deviceType = kESDSDKDeviceType_StreamDeck;
	QString m_deviceID = "UnknownID";
	bool m_deviceConnected = false;

};