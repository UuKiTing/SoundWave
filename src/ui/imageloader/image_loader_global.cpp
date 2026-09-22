#include "image_loader_global.h"

ImageLoaderGlobal::ImageLoaderGlobal() {
    m_loader = new ImageLoader;
    m_thread = new QThread;

    m_loader->moveToThread(m_thread);

    connect(m_loader, &ImageLoader::imageLoaded, this, [this](int song_id, const QString &path, QVariant var){
        emit imageLoaded(song_id, path, var);
    });

    connect(m_thread, &QThread::started, m_loader, &ImageLoader::init);
    connect(m_thread, &QThread::finished, m_loader, &QObject::deleteLater);
    connect(m_thread, &QThread::finished, m_thread, &QThread::deleteLater);

    m_thread->start();
}

ImageLoaderGlobal::~ImageLoaderGlobal()
{
    if(m_loader) m_loader->stop();
    if(m_thread){
        m_thread->quit();
        m_thread->wait();
    }
}

ImageLoaderGlobal &ImageLoaderGlobal::getInstance()
{
    static ImageLoaderGlobal instance;
    return instance;
}

void ImageLoaderGlobal::addTask(const ImageTask& task)
{
    m_loader->addTask(task);
}
