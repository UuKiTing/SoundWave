#ifndef SONG_LIST_MODEL_H
#define SONG_LIST_MODEL_H

#include "song_info.h"
#include <QAbstractListModel>

/**
 * @brief 自定义歌曲列表模型
 */
class SongListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit SongListModel(QObject *parent = nullptr);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    /**
     * @brief 添加歌曲到模型中
     * @param song 歌曲元数据
     */
    void addSong(const SongInfo& song);

    void addSongs(const QList<SongInfo>& songs);

    void removeSong(int row);

    /**
     * @brief 设置模型中某行的数据项的播放状态
     * @note 同时只有一个数据项处于正在播放状态
     * @param row 在模型中所在的行号
     * @return bool 设置成功返回 true，失败则返回 false
     */
    bool setPlayingStatus(int row);

    /**
     * @brief 设置模型中某行的数据项的收藏状态
     * @param row 在模型中所在的行号
     * @return bool 设置成功返回 true，失败则返回 false
     */
    bool setFavorite(int row, bool isCollect);

    /**
     * @brief 设置模型中某行的数据项的进度值
     * @param row 在模型中所在的行号
     * @param progress 进度值
     * @return bool 设置成功返回 true，失败则返回 false
     */
    bool setProgress(int row, int progress);

    /**
     * @brief 设置模型中某行的数据项为无效
     * @param row 在模型中所在的行号
     * @return bool 设置成功返回 true，失败则返回 false
     */
    bool setInvalid(int row);


    QPair<int, int> value(int song_id);

private:
    QList<SongInfo> m_list{}; ///< 数据项列表

    QHash<int, QPair<int, int>> m_idToRows{};

    int m_playingRow = -1; ///< 处于正在播放状态数据项的行号

    QSet<int> m_delSet;
};

#endif // SONG_LIST_MODEL_H
