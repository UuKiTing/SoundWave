#include "global.h"
#include <QPainter>
#include <QPainterPath>


QString toDurationString(int duration)
{
    QString minutes = QString("%1").arg(duration / 60, 2, 10, QChar('0'));
    QString seconds = QString("%1").arg(duration % 60, 2, 10, QChar('0'));

    return minutes + ":" + seconds;
}


QPixmap roundPixmap(const QPixmap &source, const QSize &size, int radius)
{
    if(source.isNull()){
        QPixmap p(size);
        p.fill(QColor(0xE0, 0xE0, 0xE0));
        return p;
    }

    QPixmap result(size);
    result.fill(Qt::transparent);

    QPainter painter(&result);
    painter.setRenderHint(QPainter::Antialiasing); // 开启抗锯齿

    // 圆角裁剪路径
    QPainterPath path;
    path.addRoundedRect(QRectF(0, 0, size.width(), size.height()), radius, radius);
    painter.setClipPath(path);

    // 直接画缩放后的图，裁剪路径会自动切掉圆角外的部分
    painter.drawPixmap(0, 0, source.scaled(size, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));

    return result;
}


QPixmap roundPixmap(const QString &path, const QSize &size, int radius)
{
    QPixmap source(path);

    if(source.isNull()){
        QPixmap p(size);
        p.fill(QColor(0xE0, 0xE0, 0xE0));
        return p;
    }

    QPixmap result(size);
    result.fill(Qt::transparent);

    QPainter painter(&result);
    painter.setRenderHint(QPainter::Antialiasing); // 开启抗锯齿

    // 圆角裁剪路径
    QPainterPath painterPath;
    painterPath.addRoundedRect(QRectF(0, 0, size.width(), size.height()), radius, radius);
    painter.setClipPath(painterPath);

    // 直接画缩放后的图，裁剪路径会自动切掉圆角外的部分
    painter.drawPixmap(0, 0, source.scaled(size, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));

    return result;
}

const QPixmap &defaultCover()
{
    static QPixmap pix(":/icon/cover.png");
    return pix;
}
