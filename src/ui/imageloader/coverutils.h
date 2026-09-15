#ifndef COVERUTILS_H
#define COVERUTILS_H

#include <QString>
#include <QSize>
#include <QPixmap>
#include <functional>
#include <QVariant>


namespace CoverUtils {

void loadCoverAsync(const QString& path, QSize size, int radius,
                    std::function<void(const QPixmap&)> onLoaded,
                    std::function<void()> onMissing,
                    QVariant var = QVariant());


QImage roundImage(const QImage& source, const QSize& size, int radius);


QString makeKey(const QString& path, const QSize& size, int radius);


} // namespace CoverUtils




#endif // COVERUTILS_H
