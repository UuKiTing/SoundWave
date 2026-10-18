#ifndef SONG_LIST_MODEL_H
#define SONG_LIST_MODEL_H

#include "song_info.h"
#include <QAbstractListModel>

// 自定义歌曲列表模型
class SongListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit SongListModel(QObject *parent = nullptr);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // 返回当前模型的行数
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    // 添加歌曲到模型中
    void addSong(const SongInfo& song);

    // 批量添加歌曲到模型中
    void addSongs(const QList<SongInfo>& songs);

    // 从模型中删除歌曲
    void removeSong(int row);

    // 设置模型中某行的数据项的播放状态
    bool setPlayingStatus(int row);

    // 设置模型中某行的数据项的收藏状态
    bool setFavorite(int row, bool isCollect);

    // 设置模型中某行的数据项的进度值
    bool setProgress(int row, int progress);

    // 设置模型中某行的数据项为无效
    bool setInvalid(int row);

    QPair<int, int> sameSongPair(int song_id);
private:
    QList<SongInfo> m_list{}; // 数据项列表

    QHash<int, QPair<int, int>> m_idToRows{};

    int m_playingRow = -1; // 处于正在播放状态数据项的行号

    QSet<int> m_delSet;
};

#endif // SONG_LIST_MODEL_H
