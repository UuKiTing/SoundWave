#ifndef LOGGING_H
#define LOGGING_H

#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(dbLog) // 数据库层
Q_DECLARE_LOGGING_CATEGORY(playerLog) // 播放器层
Q_DECLARE_LOGGING_CATEGORY(uiLog) // Ui层
Q_DECLARE_LOGGING_CATEGORY(mediatorLog)  // 中介层
Q_DECLARE_LOGGING_CATEGORY(appLog) // 应用层
Q_DECLARE_LOGGING_CATEGORY(modelLog)  // 模型层


void setupLogFormat();    // 设置输出格式（时间戳+分类+级别）
void setupFileLogging();  // 安装文件日志处理器（写文件+自动轮转）
void shutdownFileLogging();


#endif // LOGGING_H
