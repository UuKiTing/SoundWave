#ifndef MUSIC_DETAIL_WIDGET_H
#define MUSIC_DETAIL_WIDGET_H

#include "http_request.h"
#include <QWidget>
#include <QList>
#include <QScrollBar>
#include <QPropertyAnimation>

namespace Ui {
class MusicDetailWidget;
}

struct LyricLine{
    qint64 time;
    qint64 duration;
    QString text;
};

class MusicDetailWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MusicDetailWidget(QWidget *parent = nullptr);
    ~MusicDetailWidget();

    /** @brief 刷新歌曲详情页  */
    void flushDetail(const QModelIndex &index);

    /** @brief 读取歌词数据 */
    void getLyricsData(const QString &filePath);

    /** @brief 解析歌词  */
    QList<LyricLine> parseLyrics(const QByteArray &data);

    /** @brief 显示歌词  */
    void showLyrics(const QByteArray &data);

    /** @brief 根据播放时间获取歌词索引  */
    int getLyricIndexByTime(const QList<LyricLine> &lyricList, qint64 position);

    /** @brief 设置歌曲详情页的封面图片  */
    void setCover(int song_id, const QString& path);

signals:
    void lyricsDataLoaded(const QByteArray &data);

public slots:
    /** @brief 根据播放时间更新歌词显示  */
    void onAudioPositionChanged(qint64 position);

private:
    Ui::MusicDetailWidget *ui;

    QList<LyricLine> m_currentLyrics;

    int m_lastLyricIndex = -1;

    QPropertyAnimation *m_scrollAnimation{};

    HttpRequest *m_httpRequest; ///< http请求模块
};

#endif // MUSIC_DETAIL_WIDGET_H
