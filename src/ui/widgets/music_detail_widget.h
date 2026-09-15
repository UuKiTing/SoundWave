#ifndef MUSIC_DETAIL_WIDGET_H
#define MUSIC_DETAIL_WIDGET_H

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

    void flushDetail(const QModelIndex &index); // 刷新歌曲详情

    QList<LyricLine> parseLyricFile(const QString &filePath); // 解析歌词文件

    void showLyrics(const QList<LyricLine> &lyricList); // 显示歌词

    int getLyricIndexByTime(const QList<LyricLine> &lyricList, qint64 position); // 根据播放时间获取歌词索引

    void setCover(const QString& path);

public slots:
    void onAudioPositionChanged(qint64 position); // 根据播放时间更新歌词显示

private:
    Ui::MusicDetailWidget *ui;

    QList<LyricLine> m_currentLyrics;

    int m_lastLyricIndex = -1;

    QPropertyAnimation *m_scrollAnimation{};
};

#endif // MUSIC_DETAIL_WIDGET_H
