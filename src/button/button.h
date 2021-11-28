#pragma once

#include <QString>
#include <QJsonObject>

#include "device.h"
#include "plugin.h"

class Button {

public:
	struct CtorData {
		Device *device;
		QString action;
		QString context;
		QJsonObject payload;
	};

public:
	Button(const CtorData &d);

	virtual ~Button() {}

public:
	virtual void onPressed() {}

	virtual void update() {}

public:
	Device &device;
	const QString context;

};

