#ifndef IPLUGINMODEL_H
#define IPLUGINMODEL_H

#include <QJsonObject>
#include <QStringList>
#include <QString>
#include "esdsdkdefines.h"

class IDataModel {

public:
	IDataModel() = default;
	virtual ~IDataModel() = default;

public:
	virtual bool fromJson(const QJsonObject &json) { return false; }

	virtual bool toJson(const QJsonObject &json) { return false; }

protected:
	static inline bool containsKeys(const QJsonObject &json, QStringList &keyList) {
		return std::all_of(keyList.begin(), keyList.end(), [&json](const QString &key) { return json.contains(key); });
	}

};

#endif // IPLUGINMODEL_H
