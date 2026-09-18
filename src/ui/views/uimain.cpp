#include "uimain.h"
#include "ui_uimain.h"
#include "dbmanager.h"
#include "image_loader_global.h"
#include "cover_cache_manager.h"
#include "song_manager.h"
#include "coverutils.h"
#include "contextmenu.h"
#include <QPainter>
#include <QPainterPath>
#include <QWidgetAction>
#include <QMenu>
#include <QLabel>


UIMain::UIMain(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::UIMain)
{
    ui->setupUi(this);

    // 设置委托
    m_delegate = new StyleItemDelegate(this);
    ui->listView->setItemDelegate(m_delegate);
    ui->collectListView->setItemDelegate(m_delegate);
    ui->songListView->setItemDelegate(m_delegate);
    ui->remoteListView->setItemDelegate(m_delegate);

    // 设置列表视图的右键菜单策略
    ui->listView->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->collectListView->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->songListView->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->remoteListView->setContextMenuPolicy(Qt::CustomContextMenu);

    initVolumeMenu(); // 初始化音量菜单

    connectSignal(); // 连接信号槽
}


UIMain::~UIMain()
{
    delete ui;
}

void UIMain::connectSignal()
{
    // 连接委托的收藏信号到UIMain的收藏槽函数
    connect(m_delegate, &StyleItemDelegate::collected, this, &UIMain::collected);
    connect(m_delegate, &StyleItemDelegate::cancelCollected, this, &UIMain::cancelCollected);

    // 显示主页歌曲列表的右键菜单
    auto& contextMenu =  ContextMenu::getInstance();
    connect(ui->listView, &QListView::customContextMenuRequested, this, [this, &contextMenu](const QPoint &pos){
         contextMenu.show(ui->listView, pos);
    });
    connect(ui->collectListView, &QListView::customContextMenuRequested, this, [this, &contextMenu](const QPoint &pos){
         contextMenu.show(ui->collectListView, pos);
    });
    connect(ui->songListView, &QListView::customContextMenuRequested, this, [this, &contextMenu](const QPoint &pos){
         contextMenu.show(ui->songListView, pos);
    });
    connect(ui->remoteListView, &QListView::customContextMenuRequested, this, [this, &contextMenu](const QPoint &pos){
        contextMenu.show(ui->remoteListView, pos);
    });


    connect(&contextMenu, &ContextMenu::songlistCoverUpdated, this, &UIMain::setSonglistCover);

    connect(ImageLoaderGlobal::getInstance().loader(), &ImageLoader::imageLoaded, this, [this](const QString& path, QVariant var){
        QString str = var.toString();
        if(str == "UIMain::setSonglistCover"){
            this->setSonglistCover(path);
        }
    });

    connect(ImageLoaderGlobal::getInstance().loader(), &ImageLoader::imageLoaded, this, [this](const QString &path, QVariant var){
        QString str = var.toString();
        if(str == "UIMain::setCoverIcon"){
            this->setCoverIcon(path);
        }
    });
}


void UIMain::initVolumeMenu()
{
    m_volumeMenu = new QMenu(this);

    m_volumeSlider = new QSlider(Qt::Vertical);
    m_volumeSlider->setRange(0, 100);

    //TODO: StyleSheet可以抽象成.qss文件
    m_volumeMenu->setStyleSheet(R"(
        QSlider {
            background-color: transparent;
        }

        QSlider::groove:vertical {
            background: #E3F2FD;
            width: 4px;
        }

        QSlider::add-page:vertical {
            background: #64B5F6;
            width: 4px;
        }

        QSlider::handle:vertical {
            background: #FFFFFF;
            height: 10px;
            width: 15px;
            border-radius: 5px;
            margin: 0 -6px;
            border: 1px solid #64B5F6;
        })");


    QWidgetAction *action = new QWidgetAction(m_volumeMenu);
    action->setDefaultWidget(m_volumeSlider);

    m_volumeMenu->addAction(action);
}


void UIMain::setPlayStyle(const QModelIndex &index)
{
    if(!index.isValid()) return;

    this->setTotalDuration(toDurationString(index.data(Roles::Duration).toInt())); // 设置最大时长
    this->setCoverIcon(index.data(Roles::CoverPath).toString());  // 设置音乐封面
    this->setTitleAndArtist(index.data(Roles::Title).toString(), // 设置音乐名称和作者
                            index.data(Roles::Artist).toString());

    if(index.data(Roles::IsFavorite).toBool()){ // 设置收藏状态
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
    ui->stackedWidget->setCurrentIndex(pageIndex); // 切换堆叠窗口的页面
}


void UIMain::collectStatusToggle(bool checked)
{
    collectIconToggle(checked); // 收藏图标切换
    if(checked) emit collected(); // 发射收藏信号
    else emit cancelCollected(); // 发射取消收藏信号
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


void UIMain::setModel(QAbstractItemView *view, QAbstractItemModel *model)
{
    view->setModel(model);
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
    ui->progressSlider->setValue(position);
}


void UIMain::setCoverIcon(const QString &path)
{
    CoverUtils::loadCoverAsync(path, ui->coverBtn->size(), 5,
                               [this](const QPixmap& pix){ui->coverBtn->setIcon(pix);},
                               [this, path](){m_cover = path;},
                                "UIMain::setCoverIcon");
}


void UIMain::setVolumeValue(float volume)
{
    m_volumeSlider->setValue(volume * 100);
}


void UIMain::setTitleAndArtist(const QString &title, const QString &artist)
{
    QFontMetrics metrics(ui->titleSinger->font());

    QStringList arr = metrics.elidedText(title + "-" + artist, Qt::ElideRight, ui->titleSinger->width()).split("-");
    QString str = QString("<span vertical-align:middle;'>%1</span>"
                          "<span style='font-size:12px; vertical-align:middle;'> - %2</span>")
                      .arg(arr[0], arr.size() > 1 ? arr[1] : "");

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

void UIMain::setSonglistCover(const QString &path)
{
    CoverUtils::loadCoverAsync(path, ui->songlistCover->size(), 5,
                   [this](const QPixmap& pix){ui->songlistCover->setPixmap(pix);},
                   [this](){ui->songlistCover->setPixmap(defaultCover());},
                    "UIMain::setSonglistCover"
                   );
}

QListView *UIMain::listView()
{
    return ui->listView;
}


QListView *UIMain::collectListView()
{
    return ui->collectListView;
}

QListView *UIMain::songListView()
{
    return ui->songListView;
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

int UIMain::getPlaylistRows(const QAbstractItemModel *model)
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

QString UIMain::coverPath()
{
    return m_cover;
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

    this->setPlayStyle(SongManager::mapToSource(index)); // 设置播放样式
    emit songPlayed(index, autoPlay); // 发射播放请求信号
}

void UIMain::skipMusic(bool isNext)
{
    emit songSkipped(isNext);
}

void UIMain::on_listView_doubleClicked(const QModelIndex &index)
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
    emit modeChanged();
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
        emit showDetailWidget(true);
    }
    else{
        emit showDetailWidget(false);
    }
}


void UIMain::on_songListView_doubleClicked(const QModelIndex &index)
{
    this->doubleClickPlay(index, true);
}


void UIMain::on_playlistBtn_clicked()
{
    QAbstractItemModel *model = ui->songListView->model();
    if(model && model->rowCount() >  0){
        QModelIndex index = model->index(0, 0);
        this->doubleClickPlay(index, true);
    }
}


void UIMain::on_remoteListView_doubleClicked(const QModelIndex &index)
{
    this->doubleClickPlay(index, true);
}

