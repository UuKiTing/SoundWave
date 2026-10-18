#ifndef COVERUTILS_H
#define COVERUTILS_H

#include <QString>
#include <QSize>
#include <QPixmap>
#include <functional>
#include <QVariant>


namespace CoverUtils {

//步加载封面图片
bool loadCoverAsync(int cover_id, const QString& path, QSize size, int radius,
                    std::function<void(const QPixmap&)> onLoaded,
                    QVariant var = QVariant());


QImage roundImage(const QImage& source, const QSize& size, int radius);


QString makeKey(int cover_id, const QSize& size, int radius);


} // namespace CoverUtils




#endif // COVERUTILS_H
