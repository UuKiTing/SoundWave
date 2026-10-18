#include "uisidebar.h"
#include "ui_uisidebar.h"
#include "ui_create_playlist_dialog.h"
#include "ui_timing.h"
#include "dbmanager.h"
#include "coverutils.h"
#include "contextmenu.h"
#include "image_loader_global.h"
#include "page.h"
#include "image_utils.h"
#include <QInputDialog>
#include <QMessageBox>

UISideBar::UISideBar(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::UISideBar)
{
    ui->setupUi(this);

    m_pageBtnGroup = new QButtonGroup(this);

    m_timing = new Timing(this);
    m_createPlaylisDialog = new CreatePlaylistDialog(this);

    initSideBtn();

    initContextMenu();

    connectSignals();
}

UISideBar::~UISideBar()
{
    delete ui;
}

void UISideBar::connectSignals()
{
    connect(m_contextMenu, &QMenu::triggered, this, &UISideBar::triggerMenu);

    ContextMenu &contextMenu = ContextMenu::getInstance();

    // 更改歌单封面图片
    connect(&contextMenu, &ContextMenu::playlistCoverUpdated, this, &UISideBar::updatePlaylistCover);

    // 加载歌单的封面图片
    connect(&ImageLoaderGlobal::getInstance(), &ImageLoaderGlobal::imageLoaded, this, [this](int song_id, const QString& path, QVariant var){
        QAbstractButton* btn = var.value<QAbstractButton*>();
        if(btn) this->setPlaylistCover(song_id, path, btn);
    });

    connect(m_timing, &Timing::secondElapsed, ui->countDownBtn, &QPushButton::setText);
    connect(m_timing, &Timing::timerStarted, this, &UISideBar::showCountdownBtn);
    connect(m_timing, &Timing::timeout, this, &UISideBar::hideCountdownBtn);

    connect(ui->countDownBtn, &QPushButton::clicked, this, [this](bool checked){
        if(!checked){
            hideCountdownBtn();
            m_timing->stop();
        }
    });

    connect(m_createPlaylisDialog, &CreatePlaylistDialog::playlistAdded, this, &UISideBar::playlistAdded);
    connect(m_createPlaylisDialog, &CreatePlaylistDialog::playlistCreated, this, &UISideBar::createPlaylist);
}

void UISideBar::initSideBtn()
{
    QList<QPushButton*> buttons = ui->mainBtnWidget->findChildren<QPushButton*>();
    for (auto btn : buttons) {
        btn->setCheckable(true);
        btn->setCursor(Qt::PointingHandCursor);

        const QString &name = btn->objectName();
        if(name == "localBtn"){
            connect(btn, &QPushButton::toggled, this, &UISideBar::toggleToLocalPage);
        }
        else if(name == "collectBtn"){
            connect(btn, &QPushButton::toggled, this, &UISideBar::toggleToCollectPage);
        }
        else if(name == "networkBtn"){
            connect(btn, &QPushButton::toggled, this, &UISideBar::toggleToNetworkPage);
        }

        m_pageBtnGroup->addButton(btn);
    }

    hideCountdownBtn();
}

void UISideBar::toggleToCollectPage(bool checked)
{
    if(checked) {
        emit pageChanged(MainPage::Collect);
        ui->collectBtn->setIcon(QIcon(":/icon/love.png"));
    }
    else{
        ui->collectBtn->setIcon(QIcon(":/icon/dislove.png"));
    }
}

void UISideBar::toggleToNetworkPage(bool checked)
{
    if(checked) {
        emit pageChanged(MainPage::Remote);
        ui->networkBtn->setIcon(QIcon(":/icon/networking.png"));
    }
    else{
        ui->networkBtn->setIcon(QIcon(":/icon/network.png"));
    }
}

void UISideBar::toggleToLocalPage(bool checked)
{

    if(checked) {
        emit pageChanged(MainPage::Local);
        ui->localBtn->setIcon(QIcon(":/icon/homeSelect.png"));
    }
    else{
        ui->localBtn->setIcon(QIcon(":/icon/homeNormal.png"));
    }
}

void UISideBar::createPlaylist(const PlayListInfo &info)
{
    QVBoxLayout *layout = qobject_cast<QVBoxLayout*>(ui->songList->layout());

    QPushButton *btn = new QPushButton;

    setPlaylistCover(info.cover_id, info.coverPath, btn);

    btn->setIconSize(QSize(30, 30));
    btn->setToolTip(info.name);
    btn->setCursor(Qt::PointingHandCursor);
    btn->setProperty("playlist", QVariant::fromValue(info));
    btn->setCheckable(true);

    m_pageBtnGroup->addButton(btn);

    layout->insertWidget(layout->count() - 1, btn);

    // 点击歌单
    connect(btn, &QPushButton::clicked, [this, btn](){
        PlayListInfo info = btn->property("playlist").value<PlayListInfo>(); // 获取歌单元数据
        emit playlistUpdated(info.id); // 发射歌单更新信号
        emit playlistClicked(info); // 发射歌单点击信号
        emit pageChanged(MainPage::PlayList); // 发射页面切换信号
        m_currentSelectPlaylist = info.id;
    });

    btn->setContextMenuPolicy(Qt::CustomContextMenu);

    // 右击歌单按钮显示菜单栏
    connect(btn, &QPushButton::customContextMenuRequested, this, [btn, this, info](const QPoint &pos) {
        m_contextMenu->setProperty("playlist_id", info.id);
        btn->click();
        m_contextMenu->exec(btn->mapToGlobal(pos));
    });
}

void UISideBar::initContextMenu()
{
    m_contextMenu = new QMenu(this);
    QAction *playAction = new QAction(QIcon(":/icon/playlistRightClick/play.png"), "播放", this);
    QAction *delAction = new QAction(QIcon(":/icon/playlistRightClick/del.png"), "删除", this);
    QAction *renameAction = new QAction("重命名", this);

    playAction->setObjectName("play");
    delAction->setObjectName("del");
    renameAction->setObjectName("rename");

    m_contextMenu->addAction(playAction);
    m_contextMenu->addAction(delAction);
    m_contextMenu->addAction(renameAction);
}

void UISideBar::triggerMenu(QAction *action)
{
    int playlist_id = m_contextMenu->property("playlist_id").toInt();

    QAbstractButton* btn = findPlaylist(playlist_id);

    if(!btn) return;

    if(action->objectName() == "play"){
        emit playlistPlayed(); // 播放歌单
    }
    else if(action->objectName() == "del"){
        emit playlistDeleted(playlist_id);
    }
    else if(action->objectName() == "rename"){
        QString newName = QInputDialog::getText(this, "重命名", "请输入新名称", QLineEdit::Normal);
        PlayListInfo info = btn->property("playlist").value<PlayListInfo>();
        if(!newName.isEmpty() && newName != info.name){
            info.name = newName;
            btn->setProperty("playlist", QVariant::fromValue(info));
            btn->setToolTip(newName);

            emit playlistNameChanged(newName, playlist_id);
        }
    }
}

void UISideBar::deletePlaylist(int playlist_id)
{
    QPushButton* btn = qobject_cast<QPushButton*>(findPlaylist(playlist_id));

    int number = findPlaylistOrder(btn); // 查看当前右键的歌单在所有歌单中的序号
    if(number < 0) return;

    QList<QPushButton*> list = ui->songList->findChildren<QPushButton*>();
    if(list.isEmpty()) return;

    int next = (number + 1) % list.size(); // 计算当前右键的歌单下一个歌单的序号

    list[next]->click();
    m_pageBtnGroup->removeButton(btn);
    ui->songList->layout()->removeWidget(btn);
    btn->deleteLater();

    if(next == number){ // 如果只有一个歌单，没有下一个歌单了，就跳转到本地页
        ui->localBtn->click();
    }
}


QAbstractButton* UISideBar::findPlaylist(int playlist_id)
{
    QList<QPushButton*> buttons = ui->songList->findChildren<QPushButton*>();
    for(auto btn : buttons){
        PlayListInfo info = btn->property("playlist").value<PlayListInfo>();
        if(info.id == playlist_id){
            return btn;
        }
    }

    return nullptr;
}

int UISideBar::findPlaylistOrder(QAbstractButton *btn)
{
    QList<QPushButton*> buttons = ui->songList->findChildren<QPushButton*>();
    for(int i = 0; i < buttons.size(); ++i){
        if(buttons[i] == btn){
            return i;
        }
    }
    return -1;
}

QPushButton* UISideBar::findPlaylistBtn(int order)
{
    QList<QPushButton*> buttons = ui->songList->findChildren<QPushButton*>();
    if(order < buttons.size() && order >= 0) return buttons[order];
    else return nullptr;
}


QPushButton *UISideBar::getSideBtnOfPage(int page)
{
    switch (page) {
    case MainPage::Local: return ui->localBtn;
    case MainPage::Collect: return ui->collectBtn;
    case MainPage::Remote: return ui->networkBtn;
    default: return nullptr;
    }
}

int UISideBar::currentSelectPlaylist()
{
    return m_currentSelectPlaylist;
}

void UISideBar::updatePlaylistCover(int playlist_id, int song_id, const QString &path)
{
    QAbstractButton* btn = findPlaylist(playlist_id);

    setPlaylistCover(song_id, path, btn);

    PlayListInfo info = btn->property("playlist").value<PlayListInfo>();
    info.coverPath = path;
    btn->setProperty("playlist", QVariant::fromValue(info));
}

void UISideBar::setPlaylistCover(int song_id, const QString &path, QAbstractButton *btn)
{
    bool res = CoverUtils::loadCoverAsync(song_id, path, QSize(30, 30), 5,
                                          [btn](const QPixmap& pix){btn->setIcon(pix);},
                                          QVariant::fromValue(btn));

    if(!res) btn->setIcon(defaultCover());
}

void UISideBar::showCountdownBtn()
{
    ui->countDownBtn->setVisible(true);
    ui->placeholder->setVisible(false);
}

void UISideBar::hideCountdownBtn()
{
    ui->countDownBtn->setVisible(false);
    ui->placeholder->setVisible(true);
}

void UISideBar::createPlaylists(const QList<PlayListInfo> &list)
{
    for(const auto &info : list){
        createPlaylist(info);
    }
}

void UISideBar::on_addPlaylistBtn_clicked()
{
    int result = m_createPlaylisDialog->exec();

    if(result == QDialog::Accepted){
        m_createPlaylisDialog->addPlaylist();
    }
}

void UISideBar::on_timingBtn_clicked()
{
    int result = m_timing->exec();

    if(result == QDialog::Accepted){
        if(!m_timing->startCountdown())
            on_timingBtn_clicked();
    }
}

