#include <QCoreApplication>
#include <QCommandLineParser>

#include "plugin.h"

int main(int argc, char *argv[]) {
	QCoreApplication app(argc, argv);

	ESDConfig esdConfig;
	{
		QCommandLineParser cmdParser;
		cmdParser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);
		cmdParser.addHelpOption();

		QCommandLineOption portOption("port", "", "port", "0");
		cmdParser.addOption(portOption);

		QCommandLineOption pluginuuidOption("pluginUUID", "", "pluginUUID", "");
		cmdParser.addOption(pluginuuidOption);

		QCommandLineOption registerEventOption("registerEvent", "", "registerEvent", "");
		cmdParser.addOption(registerEventOption);

		QCommandLineOption infoOption("info", "", "info", "");
		cmdParser.addOption(infoOption);

		cmdParser.process(app);

		esdConfig.port = cmdParser.value(portOption).toInt();
		esdConfig.pluginUUID = cmdParser.value(pluginuuidOption);
		esdConfig.registerEvent = cmdParser.value(registerEventOption);
		esdConfig.info = cmdParser.value(infoOption);
	}

	Plugin plugin;
	if(!plugin.init(esdConfig))
		return -1;

	return app.exec();
}
