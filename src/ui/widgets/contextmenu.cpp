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

void ContextMenu::setPlaylistDataSource(QueryPlaylistsFunc queryPlaylists, InsertSongFunc insertSongs, SongCountFunc songCount, UpdateCoverFunc updateCover)
{
    m_queryPlaylists = queryPlaylists;
    m_insertSongs = insertSongs;
    m_songCount = songCount;
    m_updateCover = updateCover;
}

void ContextMenu::show(QListView *view, const QPoint &pos)
{
    QModelIndex index = view->indexAt(pos);
    if(index.isValid()){
        int song_id = index.data(Roles::Id).toInt();

        m_contextMenu->setProperty("song_id", song_id);  // 为右键菜单栏设置当前右键歌曲id
        m_contextMenu->setProperty("coverPath", index.data(Roles::CoverPath));  // 为右键菜单栏设置当前右键歌曲封面图片
        m_contextMenu->setProperty("row", index.row());

        QSet<int> result = m_queryPlaylists ? m_queryPlaylists(song_id) : QSet<int>();

        setPlaylistEnabled(result);

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

void ContextMenu::setActionIcon(QAction *action, const QString &filePath, int cover_id)
{
    int size = style()->pixelMetric(QStyle::PM_SmallIconSize);

    bool res = CoverUtils::loadCoverAsync(cover_id, filePath, QSize(size, size), 2,
                               [action](const QPixmap& pix){action->setIcon(pix);},
                               QVariant::fromValue(action));

    if(!res) action->setIcon(defaultCover());
}

void ContextMenu::addPlaylists(const QList<PlayListInfo> &list)
{
    for(const PlayListInfo &info : list){
        addPlaylist(info);
    }
}

bool ContextMenu::addSongToPlaylist(QAction *action)
{
    if(!action) return false;

    int song_id = m_contextMenu->property("song_id").toInt();
    int playlist_id = action->data().toInt();

    bool res = m_insertSongs ? m_insertSongs(playlist_id, song_id) : false;

    if(!res){
        return false;
    }

    QString path = m_contextMenu->property("coverPath").toString();

    // 如果歌单中的歌曲数量为 1, 则更新歌单封面图片
    int count = m_songCount ? m_songCount(playlist_id) : 0;
    res = m_updateCover ? m_updateCover(path, playlist_id, song_id) : false;


    if(count == 1 && res){
        action->setProperty("cover_id", song_id);

        setActionIcon(action, path, song_id);

        emit playlistCoverUpdated(playlist_id, song_id, path);

        emit songlistCoverUpdated(song_id, path);
    }

    return true;
}

void ContextMenu::moveSongToPlaylist(QAction *action)
{
    int song_id = m_contextMenu->property("song_id").toInt();

    if(!addSongToPlaylist(action)){
        return;
    }

    emit songRemoved(song_id, m_contextMenu->property("row").toInt());
}

QAction* ContextMenu::createAction(const PlayListInfo &info)
{
    QAction *action = new QAction(info.name);

    action->setData(info.id);

    setActionIcon(action, info.coverPath, info.cover_id);

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
            this->setActionIcon(action, path, song_id);
        }
    });

    connect(&m_downloader, &Downloader::downloadProgressChanged, this, &ContextMenu::downloadProgressChanged);
    connect(&m_downloader, &Downloader::songAdded, this, &ContextMenu::songAdded);
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

void ContextMenu::setPlaylistEnabled(const QSet<int> &set)
{
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
    m_downloader.downloadSong(index);
}
