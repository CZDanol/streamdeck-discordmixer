#pragma once

//==============================================================================
/**
@file       ESDDeviceModel.h
@brief      Defines action model used in communication with stream deck application
@copyright  (c) 2019, Aleksandar Zdravkovic
            This source code is licensed under MIT licence
**/
//==============================================================================

#include "idatamodel.h"

struct QStreamDeckAction : IDataModel {

public:
	bool fromJson(const QJsonObject &json);

public:
	QString action;
	QString context, deviceId, event;
	QJsonObject payload;

};

Q_DECLARE_METATYPE(QStreamDeckAction)