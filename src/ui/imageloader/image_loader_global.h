#ifndef IMAGE_LOADER_GLOBAL_H
#define IMAGE_LOADER_GLOBAL_H

#include "image_loader.h"
#include <QThread>
#include <QObject>


class ImageLoaderGlobal : public QObject
{
    Q_OBJECT
public:
    ImageLoaderGlobal(const ImageLoaderGlobal&&) = delete;
    ImageLoaderGlobal operator=(const ImageLoaderGlobal&&) = delete;

    static ImageLoaderGlobal& getInstance();

    const ImageLoader* loader() const;

    void addTask(const ImageTask &task);

private:
    ImageLoaderGlobal();
    ~ImageLoaderGlobal();

    ImageLoader* m_loader;
    QThread* m_thread;

};

#endif // IMAGE_LOADER_GLOBAL_H
