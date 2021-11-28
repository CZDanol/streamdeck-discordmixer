#pragma once

//==============================================================================
/**
@file       ESDDeviceModel.h
@brief      Defines device model used in communication with stream deck application
@copyright  (c) 2019, Aleksandar Zdravkovic
            This source code is licensed under MIT licence
**/
//==============================================================================

#include "idatamodel.h"

class ESDDeviceModel : IDataModel {
public:
	ESDDeviceModel() = default;
	~ESDDeviceModel() = default;

	bool fromJson(const QJsonObject &json);

	QString event, deviceId;
	QJsonObject deviceInfo;
};
