#include "coverutils.h"
#include "cover_cache_manager.h"
#include "image_loader_global.h"
#include <QPainter>
#include <QPainterPath>

namespace CoverUtils{


void loadCoverAsync(const QString& path, QSize size, int radius,
                    std::function<void(const QPixmap&)> onLoaded,
                    std::function<void()> onMissing, QVariant var)
{
    QImage* cached = CoverCacheManager::getInstance().get(CoverUtils::makeKey(path, size, radius));
    if(cached){
        onLoaded(QPixmap::fromImage(*cached));
    }
    else{
        onMissing();
        ImageLoaderGlobal::getInstance().addTask(ImageTask(path, var, size, radius));
    }
}

QImage roundImage(const QImage &source, const QSize &size, int radius)
{
    QImage result(size, QImage::Format_ARGB32_Premultiplied);
    result.fill(Qt::transparent);

    QPainter painter(&result);
    painter.setRenderHint(QPainter::Antialiasing); // 开启抗锯齿

    // 圆角裁剪路径
    QPainterPath path;
    path.addRoundedRect(QRectF(0, 0, size.width(), size.height()), radius, radius);
    painter.setClipPath(path);

    // 直接画缩放后的图，裁剪路径会自动切掉圆角外的部分
    painter.drawImage(0, 0, source.scaled(size, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
    painter.end();

    return result;
}

QString makeKey(const QString &path, const QSize &size, int radius)
{
    QString key;
    key.reserve(path.size() + 32);
    key += path;
    key += QChar('_');
    key += QString::number(size.width());
    key += QChar('_');
    key += QString::number(size.height());
    key += QChar('_');
    key += QString::number(radius);
    return key;
}

} // namespace CoverUtils
