#include "uisidebar.h"
#include "ui_uisidebar.h"
#include "ui_dialog.h"
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
    , ui_dialog(new Ui::UiDialog)
    , ui_timing(new Ui::UITiming)
{
    ui->setupUi(this);

    m_pageBtnGroup = new QButtonGroup(this);
    m_timerCheckBoxGroup = new QButtonGroup(this);

    m_dialog = new QDialog(this);
    m_dialog->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog); // 设置无边框和对话框属性
    m_dialog->setAttribute(Qt::WA_TranslucentBackground); // 设置背景透明

    m_timingDialog = new QDialog(this);
    m_timingDialog->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog); // 设置无边框和对话框属性
    m_timingDialog->setAttribute(Qt::WA_TranslucentBackground); // 设置背景透明

    ui_dialog->setupUi(m_dialog); // 将ui_dialog的UI设置到m_songListDialog中
    ui_timing->setupUi(m_timingDialog);

    initSideBtn();

    initContextMenu();

    initTimingPage();

    connectSignals();

    // 从数据库中查询用户的歌单，并创建对应的歌单
    QList<PlayListInfo> list = DbManager::getInstance().queryPlaylists();
    for(const auto &info : list){
        createPlaylist(info);
    }
}

UISideBar::~UISideBar()
{
    delete ui;
    delete ui_dialog;
    delete ui_timing;
}

void UISideBar::connectSignals()
{
    connect(ui_dialog->acceptBtn, &QPushButton::clicked, m_dialog, &QDialog::accept);
    connect(ui_dialog->cancelBtn, &QPushButton::clicked, m_dialog, &QDialog::reject);

    // 当取消按钮被点击时，清空歌单名称输入框的文本
    connect(ui_dialog->cancelBtn, &QPushButton::clicked, this, &UISideBar::clearInputBox);

    connect(m_contextMenu, &QMenu::triggered, this, &UISideBar::triggerMenu);

    ContextMenu &contextMenu = ContextMenu::getInstance();

    // 侧边栏创建歌单后，为右键菜单添加歌单
    connect(this, &UISideBar::playlistCreated, &contextMenu, &ContextMenu::addPlaylist);

    // 侧边栏删除歌单后，为右键菜单删除歌单
    connect(this, &UISideBar::playlistDeleted, &contextMenu, &ContextMenu::removePlaylist);

    // 更改歌单封面图片
    connect(&contextMenu, &ContextMenu::playlistCoverUpdated, this, &UISideBar::updatePlaylistCover);

    // 加载歌单的封面图片
    connect(&ImageLoaderGlobal::getInstance(), &ImageLoaderGlobal::imageLoaded, this, [this](int song_id, const QString& path, QVariant var){
        QAbstractButton* btn = var.value<QAbstractButton*>();
        if(btn) this->setPlaylistCover(song_id, path, btn);
    });

    connect(ui_timing->acceptBtn, &QPushButton::clicked, m_timingDialog, &QDialog::accept);
    connect(ui_timing->cancelbtn, &QPushButton::clicked, m_timingDialog, &QDialog::reject);

    connect(ui->countDownBtn, &QPushButton::clicked, this, [this](bool checked){
        if(!checked){
            this->setCountDownVisible(false);
            m_mainTimer->stop();
            m_updateTimer->stop();
        }
    });
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

    this->setCountDownVisible(false);
}

void UISideBar::initTimingPage()
{
    QList<QCheckBox*> list = ui_timing->options->findChildren<QCheckBox*>();

    for (QCheckBox *checkBox : list) {
        int minutes = 0;
        if(checkBox->text().contains("分钟")){
            minutes = checkBox->text().left(2).toInt();
        };

        m_timerCheckBoxGroup->addButton(checkBox, minutes);
    }

    ui_timing->minuteSpinBox->hide();
    ui_timing->hourSpinBox->hide();

    ui_timing->options->setDisabled(true);

    connect(ui_timing->minute, &QCheckBox::toggled, ui_timing->minuteSpinBox, &QWidget::setVisible);
    connect(ui_timing->minute, &QCheckBox::toggled, ui_timing->hourSpinBox, &QWidget::setVisible);
    connect(ui_timing->timeSwitch, &QCheckBox::toggled, this, [this](bool checked)
            {ui_timing->options->setDisabled(!checked);
    });

    m_mainTimer = new Timer(this);
    m_updateTimer = new Timer(this);
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
    // 获取歌单按钮的布局
    QVBoxLayout *layout = qobject_cast<QVBoxLayout*>(ui->songList->layout());

    QPushButton *btn = new QPushButton;

    setPlaylistCover(info.id, info.coverPath, btn);

    btn->setIconSize(QSize(30, 30));
    btn->setToolTip(info.name);
    btn->setCursor(Qt::PointingHandCursor);
    btn->setProperty("playlist", QVariant::fromValue(info));
    btn->setCheckable(true);

    m_pageBtnGroup->addButton(btn);

    int number = layout->count() - 1;

    layout->insertWidget(layout->count() - 1, btn);

    // 点击歌单
    connect(btn, &QPushButton::clicked, [this, btn, number](){
        PlayListInfo info = btn->property("playlist").value<PlayListInfo>(); // 获取歌单元数据
        emit playlistUpdated(DbManager::getInstance().queryPlaylistId(info.id)); // 发射歌单更新信号
        emit playlistClicked(info); // 发射歌单点击信号
        emit pageChanged(MainPage::PlayList); // 发射页面切换信号
        m_playlistSelectNumber = number;
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


void UISideBar::clearInputBox()
{
    QLineEdit* lineEdit = m_dialog->findChild<QLineEdit*>("lineEdit");
    lineEdit->clear();
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
        deletePlaylist(btn, playlist_id);
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

void UISideBar::deletePlaylist(QAbstractButton *btn, int playlist_id)
{
    if(!DbManager::getInstance().deletePlaylist(playlist_id)){
        return;
    }

    int number = findPlaylistOrder(btn); // 查看当前右键的歌单在所有歌单中的序号
    if(number < 0) return;

    QList<QPushButton*> list = ui->songList->findChildren<QPushButton*>();
    if(list.isEmpty()) return;

    int next = (number + 1) % list.size(); // 计算当前右键的歌单下一个歌单的序号

    list[next]->click();
    m_pageBtnGroup->removeButton(btn);
    ui->songList->layout()->removeWidget(btn);
    btn->deleteLater();

    emit playlistDeleted(playlist_id);

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

QPushButton* UISideBar::findSonglistBtn(int order)
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

int UISideBar::playlistSelectNumber()
{
    return m_playlistSelectNumber;
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
    CoverUtils::loadCoverAsync(song_id, path, QSize(30, 30), 5,
                               [btn](const QPixmap& pix){btn->setIcon(pix);},
                               [btn](){btn->setIcon(defaultCover());},
                               QVariant::fromValue(btn));
}

void UISideBar::setCountDownVisible(bool visible)
{
    ui->countDownBtn->setVisible(visible);
    ui->placeholder->setVisible(!visible);
}

void UISideBar::on_addSongBtn_clicked()
{
    int result = m_dialog->exec();

    // 获取用户输入的歌单名称
    QLineEdit* lineEdit = m_dialog->findChild<QLineEdit*>("lineEdit");
    QString name = lineEdit->text();

    // 如果用户点击了接受按钮并且输入不为空，则在数据库中创建新的歌单，并调用createPlayList方法创建对应的歌单按钮
    if(result == QDialog::Accepted && !name.isEmpty()){
        PlayListInfo info = DbManager::getInstance().createPlaylist(name);
        createPlaylist(info);
        emit playlistCreated(info);
    }

    // 清空歌单名称输入框的文本，以便下次创建歌单时输入框为空
    lineEdit->clear();
}


void UISideBar::on_timingBtn_clicked()
{
    int result = m_timingDialog->exec();

    if(result == QDialog::Accepted){
        if(ui_timing->timeSwitch->isChecked  ()){
            int sec = m_timerCheckBoxGroup->checkedId() * 60;

            if(sec == 0)
                sec = ui_timing->defineHour->value() * 60 + ui_timing->defineMinute->value() * 60;

            if(sec == 0){
                QMessageBox::warning(this, "警告⚠️", "不能为空");
                this->on_timingBtn_clicked();
            }

            m_mainTimer->createTimer(sec * 1000, true, [this](){
                emit songPlayedOrPaused();
                return true;
            });

            auto updateUI = [this](){
                int msec = m_mainTimer->remainingTime();
                if(msec > 0) {
                    this->setCountDownVisible(true);
                    ui->countDownBtn->setText(Timer::timeToString(msec));
                    return true;
                }

                this->setCountDownVisible(false);
                return false;
            };

            updateUI();
            m_updateTimer->createTimer(1000, false, updateUI);
        }
        else{

        }
    }
}

