#ifndef DBMANAGER_H
#define DBMANAGER_H

#include "song_info.h"
#include "playlist_info.h"
#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QList>
#include <QModelIndex>
#include <QMutex>

// 数据库管理器（单例）
class DbManager
{
public:
    // 禁用拷贝构造和拷贝赋值
    DbManager(const DbManager&) = delete;
    DbManager& operator=(const DbManager&) = delete;

    // 返回 DbManager单例对象
    static DbManager& getInstance();

    // 获取连接对象
    QSqlDatabase getConnection();

    // 数据库是否可以用
    bool isValid();

private:
    // 私有构造和析构函数
    explicit DbManager();
    ~DbManager();

    // 创建歌曲表
    bool createSongTable(QSqlDatabase db);

    // 创建收藏表
    bool createCollectionTable(QSqlDatabase db);

    // 创建歌单表
    bool createPlaylistsTable(QSqlDatabase db);

    // 创建歌单歌曲表
    bool createPlaylistSongsTable(QSqlDatabase db);

    bool m_isValid = true; // 数据库可用性标识

    QMutex m_mutex; // 互斥锁，用于多线程下互斥获取连接对象
};

#endif // DBMANAGER_H
