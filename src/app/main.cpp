#include "soundwave.h"
#include "logging.h"
#include "path_manager.h"
#include <QApplication>
#include <QSystemTrayIcon>
#include <QSettings>
#include <QFile>

bool loadConfig(){
    QFile file(Paths::configFilePath());

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qCWarning(appLog) << "未找到 url.config 配置文件，默认使用服务器地址:" << Url::BaseUrl;
        return false;
    }

    QTextStream in(&file);

    while(!in.atEnd()){
        QString line = in.readLine().trimmed();

        if(line.isEmpty() || line.startsWith("#")) continue;

        int pos = line.indexOf("=");
        if(pos < 0) return false;

        QString key = line.left(pos).trimmed();
        QString value = line.mid(pos + 1).trimmed();

        if(key == "BaseUrl"){
            Url::BaseUrl = value;
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

    if(!player.isInitialized()){
        qCCritical(appLog) << "应用初始化失败，退出";
        shutdownFileLogging();
        return EXIT_FAILURE;
    }

    player.show();

    int res = a.exec();

    shutdownFileLogging();

    return res;
}
