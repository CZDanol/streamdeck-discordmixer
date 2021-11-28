#pragma once

#include "idatamodel.h"

class ESDEventModel : IDataModel {
public:
	ESDEventModel() = default;
	~ESDEventModel() = default;

	bool fromJson(const QJsonObject &json);

	QString event;
	QJsonObject payload;
};
