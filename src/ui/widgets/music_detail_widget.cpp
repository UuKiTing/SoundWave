#include "music_detail_widget.h"
#include "ui_music_detail_widget.h"
#include "coverutils.h"
#include "logging.h"
#include "image_loader_global.h"
#include "model_roles.h"
#include "image_utils.h"
#include "stylesheetutils.h"
#include "path_manager.h"
#include <QModelIndex>
#include <QPixmap>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QListWidget>
#include <QMap>
#include <QCoreApplication>


MusicDetailWidget::MusicDetailWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MusicDetailWidget)
{
    ui->setupUi(this);

    m_httpRequest = new HttpRequest(this);

    m_scrollAnimation = new QPropertyAnimation(
        ui->listWidget->verticalScrollBar(),
        "value",
        this);

    m_scrollAnimation->setDuration(200);
    m_scrollAnimation->setEasingCurve(QEasingCurve::Linear);

    QString styleSheet = StyleSheetUtils::loadStyleSheet("://qss/song_detail_listview.qss");
    ui->listWidget->setStyleSheet(styleSheet);

    connect(&ImageLoaderGlobal::getInstance(), &ImageLoaderGlobal::imageLoaded, this, [this](int song_id, const QString& path, QVariant var){
        QString str = var.toString();
        if(str == "MusicDetailWidget::setCover"){
            this->setCover(song_id, path);
        }
    });

    connect(this, &MusicDetailWidget::lyricsDataLoaded, this, &MusicDetailWidget::showLyrics);
}


MusicDetailWidget::~MusicDetailWidget()
{
    delete ui;
}


void MusicDetailWidget::flushDetail(const QModelIndex &index)
{
    ui->listWidget->clear();

    this->setCover(index.data(Roles::Id).toInt(), index.data(Roles::CoverPath).toString());

    ui->title->setText(index.data(Roles::Title).toString());
    ui->artist->setText(index.data(Roles::Artist).toString());

    getLyricsData(index.data(Roles::LyricsPath).toString());

    m_lastLyricIndex = -1;
}

void MusicDetailWidget::getLyricsData(const QString &filePath)
{
    if (filePath.startsWith("http://", Qt::CaseInsensitive) ||
        filePath.startsWith("https://", Qt::CaseInsensitive)) {

        m_httpRequest->get(filePath, [this](QByteArray data){
            emit lyricsDataLoaded(data);
        });

        return;
    }

    QFile file(filePath);

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qCWarning(uiLog) << "无法打开歌词文件:" << filePath;
        return;
    }

    emit lyricsDataLoaded(file.readAll());
}

QList<LyricLine> MusicDetailWidget::parseLyrics(const QByteArray &data)
{
    QTextStream stream(data);
    stream.setEncoding(QStringConverter::Utf8);

    QRegularExpression timeRegex("\\[(\\d{2}):(\\d{2})\\.(\\d{2})\\]");

    QMap<int, LyricLine> lyricMap;

    int preMillisecond = 0;

    while(!stream.atEnd()){
        QString line = stream.readLine().trimmed();
        QRegularExpressionMatch match = timeRegex.match(line);

        int minutes = match.captured(1).toInt();
        int seconds = match.captured(2).toInt();
        int millisecond = (minutes * 60 + seconds) * 1000;

        QString text = line.mid(match.capturedEnd()).trimmed();

        if(millisecond == preMillisecond){
            LyricLine lyric;
            lyric.time = millisecond;
            lyric.text = text;

            lyricMap[millisecond] = lyric;
        }
        else{

            if(lyricMap[preMillisecond].text.isEmpty()){
                lyricMap[preMillisecond].text = text;
                lyricMap[preMillisecond].time = millisecond;
            }
            else{
                lyricMap[preMillisecond].text += "\n" + text;
            }
        }

        preMillisecond = millisecond;
    }

    return lyricMap.values();
}


void MusicDetailWidget::showLyrics(const QByteArray &data)
{
    m_currentLyrics = parseLyrics(data);

    for (const LyricLine &item : m_currentLyrics) {
        ui->listWidget->addItem(item.text);
    }
}


int MusicDetailWidget::getLyricIndexByTime(const QList<LyricLine> &lyricList, qint64 position)
{
    if(lyricList.isEmpty()) return -1;

    int left = 0, right = lyricList.size() - 1;
    int result = -1;

    while(left <= right) {
        int mid = left + (right - left) / 2;
        if(lyricList[mid].time <= position) {
            result = mid;
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }

    return result;
}

void MusicDetailWidget::setCover(int song_id, const QString &path)
{
    CoverUtils::loadCoverAsync(song_id, path, QSize(320, 320), 5,
                               [this](const QPixmap& pix){ui->cover->setPixmap(pix);},
                               [this](){ui->cover->setPixmap(defaultCover());},
                               "MusicDetailWidget::setCover");
}


void MusicDetailWidget::onAudioPositionChanged(qint64 position)
{
    int currentIndex = getLyricIndexByTime(m_currentLyrics, position);

    if (currentIndex != -1 && currentIndex != m_lastLyricIndex) {
        m_lastLyricIndex = currentIndex;

        ui->listWidget->setCurrentRow(currentIndex);

        QListWidgetItem *item = ui->listWidget->item(currentIndex);

        QScrollBar *bar = ui->listWidget->verticalScrollBar();

        int startValue = bar->value();

        // 让 Qt 算出居中位置
        ui->listWidget->scrollToItem(
            item,
            QAbstractItemView::PositionAtCenter);

        int endValue = bar->value();

        // 恢复原位置
        bar->setValue(startValue);

        // 播放动画
        m_scrollAnimation->stop();
        m_scrollAnimation->setStartValue(startValue);
        m_scrollAnimation->setEndValue(endValue);
        m_scrollAnimation->start();
    }
}







