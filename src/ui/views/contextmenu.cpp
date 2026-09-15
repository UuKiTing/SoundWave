#include "contextmenu.h"
#include "global.h"
#include "dbmanager.h"
#include "song_manager.h"

ContextMenu &ContextMenu::getInstance()
{
    static ContextMenu instance;
    return instance;
}

void ContextMenu::show(QListView *view, const QPoint &pos)
{
    QModelIndex index = view->indexAt(pos);
    if(index.isValid()){
        int song_id = index.data(Roles::Id).toInt();
        m_contextMenu->setProperty("song_id", song_id);  // 为右键菜单栏设置当前右键歌曲id
        m_contextMenu->setProperty("cover", index.data(Roles::CoverPath));  // 为右键菜单栏设置当前右键歌曲封面图片
        m_contextMenu->setProperty("row", index.row());

        updatePlaylistMenu();

        setMoveMenuVisible(view);

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

bool ContextMenu::addSongToPlaylist(QAction *action)
{
    if(!action) return false;

    int song_id = m_contextMenu->property("song_id").toInt();
    int playlist_id = action->data().toInt();
    QString path = m_contextMenu->property("cover").toString();

    if(!DbManager::getInstance().insertSongToPlaylist(playlist_id, song_id)){
        qDebug() << "添加歌曲到歌单中失败！";
        return false;
    }

    if(DbManager::getInstance().songCountInPlaylist(playlist_id) == 1){

        DbManager::getInstance().updatePlaylistCover(path, playlist_id);

        action->setIcon(QIcon(path));

        emit playlistCoverUpdated(playlist_id, path);

        emit songlistCoverUpdated(path);
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
    action->setIcon(QIcon(info.cover));
    action->setData(info.id);

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

    connect(m_del, &QAction::triggered, this, [this](){
        emit songRemoved(m_contextMenu->property("song_id").toInt(),
                         m_contextMenu->property("row").toInt());
    });

    connect(m_play, &QAction::triggered, this, [this](){
        emit songPlayed(m_contextMenu->property("row").toInt());
    });
}

ContextMenu::~ContextMenu()
{

}

ContextMenu::init()
{
    m_contextMenu = new QMenu(this);

    m_play = new QAction(QIcon(":/icon/rightClickMenu/play.png"), "播放");
    m_love = new QAction("我喜欢");
    m_del = new QAction(QIcon(":/icon/rightClickMenu/del.png"), "删除");

    m_contextMenu->addAction(m_play);
    m_contextMenu->addAction(m_love);
    m_contextMenu->addAction(m_del);

    m_addToMenu = new QMenu("添加到");
    m_addToMenu->setIcon(QIcon(":/icon/rightClickMenu/add.png"));

    m_moveToMenu = new QMenu("移动到");

    QList<PlayListInfo> list = DbManager::getInstance().queryPlaylists(1);
    for(auto &info : list){
        addPlaylist(info);
    }

    m_contextMenu->addMenu(m_addToMenu);
    m_contextMenu->addMenu(m_moveToMenu);

    m_addToMenu->setActiveAction(nullptr);
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

void ContextMenu::updatePlaylistMenu()
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
    if(id == ProxyId::Local)
        m_moveToMenu->menuAction()->setVisible(false);
    else
        m_moveToMenu->menuAction()->setVisible(true);
}
