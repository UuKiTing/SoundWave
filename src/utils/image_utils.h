#ifndef IMAGE_UTILS_H
#define IMAGE_UTILS_H

#include <QPixmap>

QPixmap roundPixmap(const QPixmap &source, const QSize &size, int radius);

const QPixmap &defaultCover();

#endif // IMAGE_UTILS_H
