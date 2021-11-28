#pragma once

#include <QString>

struct ActionType {
	static const inline QString openMixer = "cz.danol.discordmixer.openmixer";
	static const inline QString back = "cz.danol.discordmixer.back";

	static const inline QString user = "cz.danol.discordmixer.user";

	static const inline QString volumeUp = "cz.danol.discordmixer.volumeUp";
	static const inline QString voumeDown = "cz.danol.discordmixer.voumeDown";

	static const inline QString nextPage = "cz.danol.discordmixer.nextPage";
	static const inline QString previousPage = "cz.danol.discordmixer.previousPage";
};

using ActionName = QString;
using ActionContext = QString;
using DeviceID = QString;
using UserIx = int;