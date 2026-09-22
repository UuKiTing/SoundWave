#include "soundwave.h"
#include "logging.h"
#include "path_manager.h"
#include <QApplication>
#include <QSystemTrayIcon>
#include <QSettings>
#include <QFile>

bool loadConfig(){
    QFile file("url.config");

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        return false;
    }

    QTextStream in(&file);

    while(!in.atEnd()){
        QStringList list = in.readLine().split("=");
        if(list.size() == 2){
            QString key = list[0];
            QString value = list[1];

            if(key == "BaseUrl"){
                BaseUrl = value;
            }
        }
    }
    return true;
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QApplication::setHighDpiScaleFactorRoundingPolicy(
        Qt::HighDpiScaleFactorRoundingPolicy::Round);

    QCoreApplication::setOrganizationName("Luo");
    QCoreApplication::setApplicationName("SoundWave");

    qRegisterMetaType<PlayListInfo>("PlayListInfo");

    setupLogFormat();
    setupFileLogging();

    qCInfo(appLog) << "应用启动";

    loadConfig();

    if(!Paths::ensureDirectories()){
        qDebug() << "创建程序数据目录失败！";
        return EXIT_FAILURE;
    }

    SoundWave player;

    if(!player.initialize()){
        qDebug() << "init";
        return EXIT_FAILURE;
    }

    player.show();

    int res = a.exec();

    shutdownFileLogging();

    return res;
}
