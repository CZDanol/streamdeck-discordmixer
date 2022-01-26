#include <QGuiApplication>
#include <QCommandLineParser>
#include <QFile>

#include "plugin.h"

void messageLogger(QtMsgType t, const QMessageLogContext &, const QString &msg) {
	static QFile lf("log.txt"), clf("clog.txt");
	if(!lf.isOpen())
		lf.open(QIODevice::WriteOnly);

	if(!clf.isOpen() && clf.exists())
		clf.open(QIODevice::Append);

	if(lf.isOpen()) {
		lf.write(msg.toUtf8());
		lf.write("\n");
		lf.flush();
	}

	if(clf.isOpen()) {
		clf.write(msg.toUtf8());
		clf.write("\n");
		clf.flush();
	}
}

int main(int argc, char *argv[]) {
	QGuiApplication app(argc, argv);
	qInstallMessageHandler(&messageLogger);
	qDebug() << QDateTime::currentDateTime().toString() << "App start";

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
