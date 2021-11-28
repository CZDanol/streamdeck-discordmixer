#pragma once

#include <QString>

struct ActionType {
	static const inline QString openMixer = "cz.danol.discordmixer.openmixer";
	static const inline QString closeMixer = "cz.danol.discordmixer.closemixer";
	static const inline QString user = "cz.danol.discordmixer.user";
};

using ActionName = QString;
using ActionContext = QString;
using DeviceID = QString;
using UserIx = int;