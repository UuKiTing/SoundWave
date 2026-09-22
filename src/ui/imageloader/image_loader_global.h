#ifndef IMAGE_LOADER_GLOBAL_H
#define IMAGE_LOADER_GLOBAL_H

#include "image_loader.h"
#include <QThread>
#include <QObject>


class ImageLoaderGlobal : public QObject
{
    Q_OBJECT
public:
    ImageLoaderGlobal(const ImageLoaderGlobal&) = delete;
    ImageLoaderGlobal& operator=(const ImageLoaderGlobal&) = delete;
    ImageLoaderGlobal(ImageLoaderGlobal&&) = delete;
    ImageLoaderGlobal& operator=(ImageLoaderGlobal&&) = delete;

    static ImageLoaderGlobal& getInstance();

    void addTask(const ImageTask &task);

signals:
    void imageLoaded(int song_id, const QString &path, QVariant var);

private:
    ImageLoaderGlobal();
    ~ImageLoaderGlobal();

    ImageLoader* m_loader;
    QThread* m_thread;

};

#endif // IMAGE_LOADER_GLOBAL_H
