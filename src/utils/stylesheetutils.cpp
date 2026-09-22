#include "stylesheetutils.h"
#include <QFile>
#include <QDebug>

StyleSheetUtils::StyleSheetUtils() {}

QString StyleSheetUtils::loadStyleSheet(const QString &filePath)
{
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "无法打开样式文件:" << filePath;
        return {};
    }

    QString styleSheet = QString::fromUtf8(file.readAll());
    file.close();
    return styleSheet;
}
