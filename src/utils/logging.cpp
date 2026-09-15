#include "logging.h"
#include <QCoreApplication>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QMutex>
#include <QMutexLocker>
#include <QFileInfo>
#include <QThread>
#include <QQueue>
#include <QWaitCondition>

// 日志分类定义
Q_LOGGING_CATEGORY(dbLog,     "musicplayer.db")
Q_LOGGING_CATEGORY(playerLog, "musicplayer.player")
Q_LOGGING_CATEGORY(uiLog,     "musicplayer.ui")
Q_LOGGING_CATEGORY(appLog, "musicplayer.app")
Q_LOGGING_CATEGORY(modelLog, "musicplayer.model")
Q_LOGGING_CATEGORY(mediatorLog, "musicplayer.mediator")

static QFile *g_logFile = nullptr;

class LogWriter: public QThread{
public:
    void entryQueue(const QString &msg){
        QMutexLocker locker(&m_mutex);
        if(m_stop) return;
        m_queue.enqueue(msg);
        m_cond.wakeOne();
    }

    void stop(){
        QMutexLocker locker(&m_mutex);
        m_stop = true;
        m_cond.wakeAll();
    }

protected:
    void run() override{
        forever{
            QString msg;
            {
                QMutexLocker locker(&m_mutex);
                while(m_queue.isEmpty() && !m_stop){
                    m_cond.wait(&m_mutex);
                }

                if(m_queue.isEmpty()) break;
                msg = m_queue.dequeue();
            }


            if (g_logFile && g_logFile->isOpen()) {
                QTextStream stream(g_logFile);
                stream << msg << "\n";
                stream.flush();
            }
#ifndef QT_NO_DEBUG
            fprintf(stderr, "%s\n", msg.toLocal8Bit().constData());
            fflush(stderr);
#endif
        }
    }


private:
    QQueue<QString> m_queue;
    QMutex m_mutex;
    QWaitCondition m_cond;
    bool m_stop = false;
};

static LogWriter g_writer;


void setupLogFormat()
{
    qSetMessagePattern(
        "[%{time yyyy-MM-dd hh:mm:ss.zzz}] "
        "[%{category}] "
        "[%{type}] "
        "%{message}"
#ifndef QT_NO_DEBUG
        "  (%{file}:%{line})"   // Debug 构建附带文件名和行号
#endif
        );
}

static void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    g_writer.entryQueue(qFormatLogMessage(type, context, msg));
}


void setupFileLogging()
{
    // 日志目录：QStandardPaths::AppDataLocation
    // QString logDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    // if (logDir.isEmpty())
    //     logDir = QCoreApplication::applicationDirPath();

    QString logDir = QCoreApplication::applicationDirPath();

    QDir dir(logDir);
    if (!dir.exists())
        dir.mkpath(logDir);

    // 清理旧日志（保留最近 5 个）
    QStringList filters;
    filters << "MusicPlayer_*.log";
    QFileInfoList oldFiles = dir.entryInfoList(filters, QDir::Files, QDir::Time);

    while (oldFiles.size() >= 5) {
        QFile::remove(oldFiles.last().absoluteFilePath());
        oldFiles.removeLast();
    }

    // 创建当天日志文件
    QString dateStr = QDateTime::currentDateTime().toString("yyyy-MM-dd");
    g_logFile = new QFile(dir.filePath(QString("MusicPlayer_%1.log").arg(dateStr)));
    if (!g_logFile->open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        delete g_logFile;
        g_logFile = nullptr;
        return;
    }

    QTextStream stream(g_logFile);
    stream << "\n========================================\n"
           << "MusicPlayer 启动 - "
           << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")
           << "\n========================================\n\n";
    stream.flush();

    g_writer.start();
    qInstallMessageHandler(messageHandler);
}

void shutdownFileLogging()
{
    qInstallMessageHandler(nullptr);
    g_writer.stop();
    g_writer.wait();
    delete g_logFile;
    g_logFile = nullptr;
}
