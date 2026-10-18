#include "uimain.h"
#include "ui_uimain.h"
#include "dbmanager.h"
#include "image_loader_global.h"
#include "cover_cache_manager.h"
#include "song_manager.h"
#include "coverutils.h"
#include "contextmenu.h"
#include "model_roles.h"
#include "image_utils.h"
#include "stylesheetutils.h"
#include <QPainter>
#include <QPainterPath>
#include <QWidgetAction>
#include <QMenu>
#include <QLabel>
#include <QFile>

UIMain::UIMain(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::UIMain)
{
    ui->setupUi(this);

    // 设置委托
    m_delegate = new StyleItemDelegate(this);
    ui->localListView->setItemDelegate(m_delegate);
    ui->collectListView->setItemDelegate(m_delegate);
    ui->playlistView->setItemDelegate(m_delegate);
    ui->remoteListView->setItemDelegate(m_delegate);

    // 设置列表视图的右键菜单策略
    ui->localListView->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->collectListView->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->playlistView->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->remoteListView->setContextMenuPolicy(Qt::CustomContextMenu);

    initVolumeMenu();

    connectSignals();
    forwardSignals();

    QString styleSheet = StyleSheetUtils::loadStyleSheet("://qss/scroll_style.qss");
    qApp->setStyleSheet(styleSheet);
}


UIMain::~UIMain()
{
    delete ui;
}

void UIMain::connectSignals()
{
    connect(m_delegate, &StyleItemDelegate::songCollected, this, &UIMain::songCollected);

    const QList<QListView*> listViews = {
        ui->localListView,
        ui->collectListView,
        ui->playlistView,
        ui->remoteListView
    };

    for (QListView *view : listViews) {
        connect(view, &QListView::customContextMenuRequested,
                this, [this, view](const QPoint &pos) {
                    ContextMenu::getInstance().show(view, pos);
                });
    }

    connect(&ContextMenu::getInstance(), &ContextMenu::songlistCoverUpdated, this, &UIMain::setSonglistCover);

    auto &imageLoaderGlobal = ImageLoaderGlobal::getInstance();
    connect(&imageLoaderGlobal, &ImageLoaderGlobal::imageLoaded, this, [this](int song_id, const QString& path, QVariant var){
        QString str = var.toString();
        if(str == "UIMain::setSonglistCover"){
            this->setSonglistCover(song_id, path);
        }
        else if(str == "UIMain::setCoverIcon"){
            this->setCoverIcon(song_id, path);
        }
    });
}

void UIMain::forwardSignals()
{
    connect(ui->progressSlider, &QSlider::sliderPressed, this, &UIMain::sliderPressed);
    connect(ui->progressSlider, &QSlider::sliderReleased, this, &UIMain::sliderReleased);
    connect(ui->progressSlider, &QSlider::valueChanged, this, &UIMain::progressChanged);
}

void UIMain::initVolumeMenu()
{
    m_volumeMenu = new QMenu(this);

    m_volumeSlider = new QSlider(Qt::Vertical);
    m_volumeSlider->setRange(0, 100);

    QString styleSheet = StyleSheetUtils::loadStyleSheet("://qss/volume_slider.qss");
    m_volumeMenu->setStyleSheet(styleSheet);

    QWidgetAction *action = new QWidgetAction(m_volumeMenu);
    action->setDefaultWidget(m_volumeSlider);

    m_volumeMenu->addAction(action);
}

void UIMain::playSonglist(QAbstractItemModel *model)
{
    if(model && model->rowCount() >  0){
        QModelIndex index = model->index(0, 0);
        this->doubleClickPlay(index, true);
    }
}


void UIMain::setPlayStyle(const QModelIndex &index)
{
    if(!index.isValid()) return;

    int duration = index.data(Roles::Duration).toInt();
    this->setTotalDuration(toDurationString(duration)); // 设置最大时长

    int song_id = index.data(Roles::Id).toInt();
    QString cover_path = index.data(Roles::CoverPath).toString();
    this->setCoverIcon(song_id, cover_path); // 设置歌曲封面

    QString title = index.data(Roles::Title).toString();
    QString artist = index.data(Roles::Artist).toString();
    this->setTitleAndArtist(title, artist); // 设置歌曲名称和作者


    bool isCollect = index.data(Roles::IsFavorite).toBool();
    if(isCollect){ // 设置收藏状态
        ui->loveBtn->setIcon(QIcon(":/icon/love.png"));
        ui->loveBtn->setChecked(true);
    }
    else{
        ui->loveBtn->setIcon(QIcon(":/icon/dislove.png"));
        ui->loveBtn->setChecked(false);
    }
}

void UIMain::switchStackedWidget(int pageIndex)
{
    ui->stackedWidget->setCurrentIndex(pageIndex); // 切换主页面
}

void UIMain::collectStatusToggle(bool checked)
{
    collectIconToggle(checked);
    emit songCollected(checked);
}

void UIMain::collectIconToggle(bool isFavo)
{
    if(isFavo) {
        ui->loveBtn->setChecked(true);
        ui->loveBtn->setIcon(QIcon(":/icon/love.png"));
    }
    else{
        ui->loveBtn->setChecked(false);
        ui->loveBtn->setIcon(QIcon(":/icon/dislove.png"));
    }
}

void UIMain::setPlayBtnIcon(QMediaPlayer::PlaybackState state)
{
    if(state == QMediaPlayer::PlayingState){
        ui->playBtn->setIcon(QIcon(":/icon/play.png"));
    }
    else if(state == QMediaPlayer::PausedState){
        ui->playBtn->setIcon(QIcon(":/icon/pause.png"));
    }
}


void UIMain::setCurDuration(qint64 position)
{
    ui->curDuration->setText(toDurationString(position / 1000));
}


void UIMain::setTotalDuration(const QString &durationString)
{
    ui->totalDuration->setText(durationString);
}


void UIMain::setProgressSliderRange(qint64 duration)
{
    ui->progressSlider->setRange(0, duration);
}


void UIMain::setProgressValue(qint64 position)
{
    if(!m_isDragging) ui->progressSlider->setValue(position);
}

void UIMain::progressDragging()
{
    m_isDragging = true;
}

void UIMain::progressNodragging()
{
    m_isDragging = false;
    setProgressValue(ui->progressSlider->value());
}

void UIMain::setCoverIcon(int song_id, const QString &path)
{
    bool res = CoverUtils::loadCoverAsync(song_id, path, ui->coverBtn->size(), 5,
                               [this](const QPixmap& pix){ui->coverBtn->setIcon(pix);},
                                "UIMain::setCoverIcon");
    if(!res) ui->coverBtn->setIcon(defaultCover());
}

void UIMain::setVolumeValue(float volume)
{
    m_volumeSlider->setValue(volume * 100);
}


void UIMain::setTitleAndArtist(const QString &title, const QString &artist)
{
    QString str = QString("<span>%1</span>"
                          "<span style='color: grey; font-size: 12px;'> - %2</span>")
                      .arg(title, artist);

    ui->titleSinger->setText(str);
}


void UIMain::setCurrentIndex(const QModelIndex &index)
{
    QListView* listView = ui->stackedWidget->currentWidget()->findChild<QListView*>();
    if(listView->model() == index.model()){
        listView->setCurrentIndex(index);
    }
}

void UIMain::setPlaylistName(const QString &name)
{
    ui->playlistName->setText(name);
}

void UIMain::setSonglistCover(int song_id, const QString &path)
{
    bool res = CoverUtils::loadCoverAsync(song_id, path, ui->songlistCover->size(), 5,
                   [this](const QPixmap& pix){ui->songlistCover->setPixmap(pix);},
                    "UIMain::setSonglistCover"
                   );
    if(!res) ui->songlistCover->setPixmap(defaultCover());
}

QListView *UIMain::localListView()
{
    return ui->localListView;
}


QListView *UIMain::collectListView()
{
    return ui->collectListView;
}

QListView *UIMain::playlistView()
{
    return ui->playlistView;
}

QListView *UIMain::remoteListView()
{
    return ui->remoteListView;
}


QSlider *UIMain::progressSlider()
{
    return ui->progressSlider;
}


QSlider *UIMain::volumeSlider()
{
    return m_volumeSlider;
}


int UIMain::progressValue()
{
    return ui->progressSlider->value();
}

int UIMain::currentPage()
{
    return ui->stackedWidget->currentIndex();
}

QFrame *UIMain::controlBar()
{
    return ui->controlBar;
}

int UIMain::geListRows(const QAbstractItemModel *model)
{
    if(!model) return -1;
    return model->rowCount();
}

QPushButton *UIMain::playlistBtn()
{
    return ui->playlistBtn;
}

QListView *UIMain::currentListView()
{
    QListView* listview = ui->stackedWidget->currentWidget()->findChild<QListView*>();
    if(listview) return listview;

    return nullptr;
}

void UIMain::changePlayMode(PlayMode mode)
{
    if(mode == PlayMode::Loop){
        ui->modeBtn->setIcon(QIcon(":/icon/loop.png"));
    }
    else if(mode == PlayMode::Random){
        ui->modeBtn->setIcon(QIcon(":/icon/random.png"));
    }
    else if(mode == PlayMode::Single){
        ui->modeBtn->setIcon(QIcon(":/icon/single.png"));
    }
}


void UIMain::doubleClickPlay(const QModelIndex &index, bool autoPlay)
{
    if (!index.isValid()) return;

    this->setPlayStyle(SongManager::mapToSource(index));
    emit songPlayed(index, autoPlay);
}

void UIMain::skipMusic(bool isNext)
{
    emit songSkipped(isNext);
}

void UIMain::updatePlaylist(const PlayListInfo &info)
{
    this->setPlaylistName(info.name);
    this->setSonglistCover(info.id, info.coverPath);
    ui->playlistBtn->setDisabled(ui->playlistView->model()->rowCount() == 0);
}

void UIMain::on_localListView_doubleClicked(const QModelIndex &index)
{
    this->doubleClickPlay(index, true);
}

void UIMain::on_collectListView_doubleClicked(const QModelIndex &index)
{
    this->doubleClickPlay(index, true);
}


void UIMain::on_playBtn_clicked()
{
    emit songPlayedOrPaused();
}


void UIMain::on_modeBtn_clicked()
{
    emit playModeChanged();
}


void UIMain::on_nextBtn_clicked()
{
    this->skipMusic(true);
}


void UIMain::on_lastBtn_clicked()
{
    this->skipMusic(false);
}

void UIMain::on_volumeBtn_clicked()
{
    m_volumeMenu->popup(ui->volumeBtn->mapToGlobal(QPoint(0, -m_volumeMenu->sizeHint().height())));
}

void UIMain::on_loveBtn_clicked(bool checked)
{
    this->collectStatusToggle(checked);
}


void UIMain::on_coverBtn_toggled(bool checked)
{
    if(checked){
        emit detailWidgetShowed(true);
    }
    else{
        emit detailWidgetShowed(false);
    }
}


void UIMain::on_playlistView_doubleClicked(const QModelIndex &index)
{
    this->doubleClickPlay(index, true);
}


void UIMain::on_playlistBtn_clicked()
{
    playSonglist(ui->playlistView->model());
}


void UIMain::on_remoteListView_doubleClicked(const QModelIndex &index)
{
    this->doubleClickPlay(index, true);
}


void UIMain::on_collectlistBtn_clicked()
{
    playSonglist(ui->collectListView->model());
}


void UIMain::on_locallistBtn_clicked()
{
    playSonglist(ui->localListView->model());
}

