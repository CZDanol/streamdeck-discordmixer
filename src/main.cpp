#include <QGuiApplication>
#include <QCommandLineParser>
#include <QFile>

#include "plugin.h"

void messageLogger(QtMsgType t, const QMessageLogContext &, const QString &msg) {
	static QFile f("log.txt");
	if(!f.isOpen())
		f.open(QIODevice::WriteOnly);

	f.write(msg.toUtf8());
	f.write("\n");
	f.flush();
}

int main(int argc, char *argv[]) {
	QGuiApplication app(argc, argv);
	qInstallMessageHandler(&messageLogger);
	qDebug() << "App start";

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
	const bool r = plugin.init(esdConfig);

	return app.exec();
}
