#pragma once

//==============================================================================
/**
@file       ESDPluginModel.h
@brief      Stores informations about plugin
@copyright  (c) 2019, Aleksandar Zdravkovic
            This source code is licensed under MIT licence
**/
//==============================================================================


#include <QString>
#include "idatamodel.h"

class ESDPluginModel : IDataModel {

public:
	ESDPluginModel() = default;
	ESDPluginModel(int port, QString pluginUUID, QString registerEvent, QString info);
	~ESDPluginModel() = default;

public:
	bool validateParameters();
	bool fromJson(const QJsonObject &json);
	bool toJson(QJsonObject &json);

public:
	int port;
	QString pluginUUID, registerEvent, info;

};
