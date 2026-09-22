#include "contextmenu.h"
#include "model_roles.h"
#include "dbmanager.h"
#include "song_manager.h"
#include "path_manager.h"
#include "coverutils.h"
#include "image_utils.h"
#include "image_loader_global.h"
#include <QDir>
#include <QEventLoop>
#include <QFutureWatcher>
#include <QtConcurrent>

ContextMenu &ContextMenu::getInstance()
{
    static ContextMenu instance;
    return instance;
}

void ContextMenu::show(QListView *view, const QPoint &pos)
{
    QModelIndex index = view->indexAt(pos);
    if(index.isValid()){
        m_contextMenu->setProperty("song_id", index.data(Roles::Id).toInt());  // 为右键菜单栏设置当前右键歌曲id
        m_contextMenu->setProperty("coverPath", index.data(Roles::CoverPath));  // 为右键菜单栏设置当前右键歌曲封面图片
        m_contextMenu->setProperty("row", index.row());

        setPlaylistEnabled();
        setMoveMenuVisible(view);
        setDownloadActionVisible(view);
        setDelActionVisible(view);

        setLoveActionIcon(index.data(Roles::IsFavorite).toBool());

        m_curIndex = index;

        m_contextMenu->exec(view->viewport()->mapToGlobal(pos));  // 显示右键菜单
    }
}

void ContextMenu::updatePlaylistName(const QString &name, int palylist_id)
{
    for(auto action : m_addToMenu->actions()){
        if(action->data().toInt() == palylist_id){
            action->setText(name);
        }
    }
}

void ContextMenu::setActionIcon(QAction *action, const QString &filePath)
{
    int size = style()->pixelMetric(QStyle::PM_SmallIconSize);
    int song_id = action->data().toInt();
    CoverUtils::loadCoverAsync(song_id, filePath, QSize(size, size), 2,
                               [action](const QPixmap& pix){action->setIcon(pix);},
                               [action](){action->setIcon(defaultCover());},
                               QVariant::fromValue(action));
}

bool ContextMenu::addSongToPlaylist(QAction *action)
{
    if(!action) return false;

    int song_id = m_contextMenu->property("song_id").toInt();
    int playlist_id = action->data().toInt();

    if(!DbManager::getInstance().insertSongToPlaylist(playlist_id, song_id)){
        qDebug() << "添加歌曲到歌单中失败！";
        return false;
    }

    QString path = m_contextMenu->property("coverPath").toString();

    // 如果歌单中的歌曲数量为 1, 则更新歌单封面图片
    if(DbManager::getInstance().songCountInPlaylist(playlist_id) == 1){

        DbManager::getInstance().updatePlaylistCover(path, playlist_id);

        setActionIcon(action, path);

        emit playlistCoverUpdated(playlist_id, song_id, path);

        emit songlistCoverUpdated(song_id, path);
    }

    return true;
}

void ContextMenu::moveSongToPlaylist(QAction *action)
{
    int song_id = m_contextMenu->property("song_id").toInt();

    if(!this->addSongToPlaylist(action)){
        return;
    }

    emit songRemoved(song_id, m_contextMenu->property("row").toInt());
}

QAction* ContextMenu::createAction(const PlayListInfo &info)
{
    QAction *action = new QAction(info.name);

    action->setData(info.id);

    setActionIcon(action, info.coverPath);

    return action;
}

void ContextMenu::addPlaylist(const PlayListInfo &info)
{
    QAction* action1 = createAction(info);
    QAction* action2 = createAction(info);

    m_addToMenu->addAction(action1);
    m_moveToMenu->addAction(action2);

    syncConnect(action1, action2);
    syncConnect(action2, action1);
}

void ContextMenu::removePlaylist(int playlist_id)
{
    QList<QAction*> addList = m_addToMenu->actions();
    QList<QAction*> moveList = m_moveToMenu->actions();

    for(int i = 0; i < addList.size(); ++i) {
        if(addList[i]->data().toInt() == playlist_id){
            m_addToMenu->removeAction(addList[i]);
            m_moveToMenu->removeAction(moveList[i]);
        }
    }
}

ContextMenu::ContextMenu()
{
    init();

    connect(m_addToMenu, &QMenu::triggered, this, &ContextMenu::addSongToPlaylist);

    connect(m_moveToMenu, &QMenu::triggered, this, &ContextMenu::moveSongToPlaylist);

    connect(m_delAction, &QAction::triggered, this, [this](){
        emit songRemoved(m_contextMenu->property("song_id").toInt(),
                         m_contextMenu->property("row").toInt());
    });

    connect(m_playAction, &QAction::triggered, this, [this](){
        emit songPlayed(m_contextMenu->property("row").toInt());
    });

    connect(m_loveAction, &QAction::triggered, this, [this](){
        bool isFavo = m_loveAction->property("isFavo").toBool();
        emit songCollected(!isFavo, m_curIndex);
    });

    connect(m_downloadAction, &QAction::triggered, this, [this](){
        this->downloadSong(m_curIndex);
    });

    connect(&ImageLoaderGlobal::getInstance(), &ImageLoaderGlobal::imageLoaded, this, [this](int song_id, const QString& path, QVariant var){
        QAction *action = var.value<QAction*>();
        if(action){
            this->setActionIcon(action, path);
        }
    });
}

ContextMenu::~ContextMenu()
{

}

void ContextMenu::init()
{
    m_contextMenu = new QMenu(this);

    m_playAction = new QAction(QIcon(":/icon/rightClickMenu/play.png"), "播放");
    m_loveAction = new QAction("我喜欢");
    m_delAction = new QAction(QIcon(":/icon/rightClickMenu/del.png"), "删除");
    m_downloadAction = new QAction(QIcon("://icon/rightClickMenu/download.png"), "下载");

    m_contextMenu->addAction(m_playAction);
    m_contextMenu->addAction(m_loveAction);
    m_contextMenu->addAction(m_downloadAction);
    m_contextMenu->addAction(m_delAction);

    m_addToMenu = new QMenu("添加到");
    m_addToMenu->setIcon(QIcon(":/icon/rightClickMenu/add.png"));

    m_moveToMenu = new QMenu("移动到");

    QList<PlayListInfo> list = DbManager::getInstance().queryPlaylists();
    for(const auto &info : list){
        addPlaylist(info);
    }

    m_contextMenu->addMenu(m_addToMenu);
    m_contextMenu->addMenu(m_moveToMenu);
}

void ContextMenu::syncConnect(QAction *src, QAction *dist)
{
    connect(src, &QAction::changed, this, [this, dist, src](){
        if(m_syncing) return;

        m_syncing = true;

        if(src->icon().cacheKey() != dist->icon().cacheKey())
            dist->setIcon(src->icon());

        if(src->text() != dist->text())
            dist->setText(src->text());

        m_syncing = false;
    });
}

void ContextMenu::setPlaylistEnabled()
{
    int song_id = m_contextMenu->property("song_id").toInt();
    QSet<int> set = DbManager::getInstance().findPlaylistsBySong(song_id);

    QList<QAction*> addActions = m_addToMenu->actions();
    QList<QAction*> moveActions = m_moveToMenu->actions();

    for (int i = 0; i < addActions.size(); ++i) {
        int playlist_id = addActions[i]->data().toInt();

        bool enable = !set.contains(playlist_id);

        addActions[i]->setEnabled(enable);
        moveActions[i]->setEnabled(enable);
    }
}

void ContextMenu::setMoveMenuVisible(QListView *view)
{
    const QSortFilterProxyModel *proxyModel = qobject_cast<QSortFilterProxyModel*>(view->model());
    if(!proxyModel) return;

    ProxyId id = proxyModel->property("proxyId").value<ProxyId>();
    if(id == ProxyId::Local || id == ProxyId::Remote)
        m_moveToMenu->menuAction()->setVisible(false);
    else
        m_moveToMenu->menuAction()->setVisible(true);
}

void ContextMenu::setDownloadActionVisible(QListView *view)
{
    const QSortFilterProxyModel *proxyModel = qobject_cast<QSortFilterProxyModel*>(view->model());
    if(!proxyModel) return;

    ProxyId id = proxyModel->property("proxyId").value<ProxyId>();
    if(id == ProxyId::Remote)
        m_downloadAction->setVisible(true);
    else
        m_downloadAction->setVisible(false);
}

void ContextMenu::setLoveActionIcon(bool isFavo)
{
    if(isFavo)
        m_loveAction->setIcon(QIcon("://icon/love.png"));
    else
        m_loveAction->setIcon(QIcon("://icon/dislove.png"));

    m_loveAction->setProperty("isFavo", isFavo);
}

void ContextMenu::setDelActionVisible(QListView *view)
{
    const QSortFilterProxyModel *proxyModel = qobject_cast<QSortFilterProxyModel*>(view->model());
    if(!proxyModel) return;

    ProxyId id = proxyModel->property("proxyId").value<ProxyId>();
    if(id == ProxyId::Remote)
        m_delAction->setVisible(false);
    else
        m_delAction->setVisible(true);
}

void ContextMenu::downloadSong(QModelIndex index)
{
    QFuture<int> future = QtConcurrent::run([index, this](QPromise<int> &promise){
        if(!index.isValid()) return;

        QString audioPath = index.data(Roles::AudioPath).toString();
        QString audioFileName = audioPath.split("/").back();
        QString coverPath = index.data(Roles::CoverPath).toString();
        QString coverFileName = coverPath.split("/").back();
        QString lyricsPath = index.data(Roles::LyricsPath).toString();
        QString lyricsFileName = lyricsPath.split("/").back();

        promise.setProgressRange(0, 3);

        HttpRequest downloader;
        auto download = [&](const QString &url, const QString &filePath){
            QEventLoop loop;
            downloader.get(url, [&](QByteArray data){

                if(!QFile::exists(filePath)){
                    QFile file(filePath);
                    if (file.open(QIODevice::WriteOnly)) {
                        file.write(data);
                        file.close();
                    }
                    else{
                        qWarning() << "打开" << filePath << "文件失败!";
                    }
                }

                loop.quit();
            });
            loop.exec();
        };

        download(audioPath, Paths::songAudioDir(audioFileName));
        promise.addResult(1);

        download(coverPath, Paths::songImagesDir(coverFileName));
        promise.addResult(2);

        download(lyricsPath, Paths::songLyricsDir(lyricsFileName));
        promise.addResult(3);

        qDebug() << "全部下载完毕";

        QList<int> collectList = DbManager::getInstance().queryCollectSongs();

        SongInfo song;

        song.id = index.data(Roles::Id).toInt();
        song.source = SongSource::Local;
        song.title = index.data(Roles::Title).toString();
        song.artist = index.data(Roles::Artist).toString();
        song.duration = index.data(Roles::Duration).toInt();
        song.durationString = toDurationString(song.duration);
        song.audioPath = Paths::songAudioDir(audioFileName);
        song.coverPath = Paths::songImagesDir(coverFileName);
        song.lyricsPath = Paths::songLyricsDir(lyricsFileName);
        song.isFavorite = collectList.contains(song.id);

        if(!DbManager::getInstance().appendSong(song)){
            qDebug() << "添加歌曲失败!";
            return;
        };

        QMetaObject::invokeMethod(this, [this, song](){
            emit songAppended(song);
        });
    });

    auto *watcher = new QFutureWatcher<int>(this);

    connect(watcher, &QFutureWatcher<int>::finished, watcher, &QObject::deleteLater);
    connect(watcher, &QFutureWatcher<int>::finished, this, [this, index](){
        QTimer::singleShot(200, [this, index](){
            emit progressChanged(index, -1);
        });
    });

    connect(watcher, &QFutureWatcher<int>::resultReadyAt, this, [this, watcher, index](int resultIndex){
        int progressValue = watcher->resultAt(resultIndex);
        emit progressChanged(index, progressValue);
    });

    watcher->setFuture(future);
}
