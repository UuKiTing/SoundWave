#include "musicplayer.h"
#include "logging.h"
#include <QApplication>
#include <QSystemTrayIcon>
#include <QSettings>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QApplication::setHighDpiScaleFactorRoundingPolicy(
        Qt::HighDpiScaleFactorRoundingPolicy::Round);

    QCoreApplication::setOrganizationName("Luo");
    QCoreApplication::setApplicationName("MusicPlayer");

    qRegisterMetaType<PlayListInfo>("PlayListInfo");

    setupLogFormat();
    setupFileLogging();

    qCInfo(appLog) << "应用启动";

    MusicPlayer player;
    player.show();

    int res = a.exec();

    shutdownFileLogging();

    return res;
}
