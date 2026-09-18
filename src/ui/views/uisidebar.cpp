#include "uisidebar.h"
#include "ui_uisidebar.h"
#include "ui_dialog.h"
#include "dbmanager.h"
#include "coverutils.h"
#include "contextmenu.h"
#include "image_loader_global.h"
#include <QInputDialog>

UISideBar::UISideBar(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::UISideBar)
    , m_dialog(new Ui::Dialog)
{
    ui->setupUi(this);

    // 设置按钮组
    m_group = new QButtonGroup(this);

    m_createSonglistDialog = new QDialog(this);
    m_createSonglistDialog->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog); // 设置无边框和对话框属性
    m_createSonglistDialog->setAttribute(Qt::WA_TranslucentBackground); // 设置背景透明

    m_dialog->setupUi(m_createSonglistDialog); // 将m_dialog的UI设置到m_songListDialog中

    initSideBtn();

    initContextMenu();

    connectSignals();

    // 从数据库中查询用户的歌单，并创建对应的歌单按钮
    QList<PlayListInfo> list = DbManager::getInstance().queryPlaylists();
    for(const auto &info : list){
        createPlaylist(info);
    }
}

UISideBar::~UISideBar()
{
    delete ui;
    delete m_dialog;
}

void UISideBar::connectSignals()
{
    // 创建歌单对话框的接受和取消按钮的点击事件连接
    connect(m_dialog->acceptBtn, &QPushButton::clicked, m_createSonglistDialog, &QDialog::accept);
    connect(m_dialog->cancelBtn, &QPushButton::clicked, m_createSonglistDialog, &QDialog::reject);

    // 当取消按钮被点击时，清空歌单名称输入框的文本
    connect(m_dialog->cancelBtn, &QPushButton::clicked, this, &UISideBar::clearInputBox);

    // 添加歌曲到歌单中
    connect(m_contextMenu, &QMenu::triggered, this, &UISideBar::rightClickPlaylist);


    ContextMenu &contextMenu = ContextMenu::getInstance();

    // 侧边栏创建歌单，为右键菜单添加歌单
    connect(this, &UISideBar::playlistCreated, &contextMenu, &ContextMenu::addPlaylist);


    // 侧边栏删除歌单，为右键菜单删除歌单
    connect(this, &UISideBar::playlistDeleted, &contextMenu, &ContextMenu::removePlaylist);

    // 更改歌单封面图片
    connect(&contextMenu, &ContextMenu::playlistCoverUpdated, this, &UISideBar::updatePlaylistCover);

    // 加载歌单的封面图片
    connect(ImageLoaderGlobal::getInstance().loader(), &ImageLoader::imageLoaded, this, [this](const QString& path, QVariant var){
        QPushButton* btn = var.value<QPushButton*>();
        if(btn) this->setPlaylistCover(path, btn);
    });

}

void UISideBar::initSideBtn()
{
    QList<QPushButton*> buttons = ui->mainBtnWidget->findChildren<QPushButton*>();
    for (auto btn : buttons) {
        btn->setCheckable(true);
        btn->setCursor(Qt::PointingHandCursor);

        m_group->addButton(btn);

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
    }
}

void UISideBar::initInputDialog()
{

}

void UISideBar::toggleToCollectPage(bool checked)
{
    if(checked) {
        emit pageChanged(Page::Collect);
        ui->collectBtn->setIcon(QIcon(":/icon/love.png"));
    }
    else{
        ui->collectBtn->setIcon(QIcon(":/icon/dislove.png"));
    }
}

void UISideBar::toggleToNetworkPage(bool checked)
{
    if(checked) {
        emit pageChanged(Page::NetWork);
        ui->networkBtn->setIcon(QIcon(":/icon/networking.png"));
    }
    else{
        ui->networkBtn->setIcon(QIcon(":/icon/network.png"));
    }
}

void UISideBar::toggleToLocalPage(bool checked)
{

    if(checked) {
        emit pageChanged(Page::Local);
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

    // 创建一个新的歌单按钮，设置其图标、提示信息和属性，然后将按钮添加到按钮组和布局中，并连接按钮的点击事件以发射相应的信号
    QPushButton *btn = new QPushButton;

    setPlaylistCover(info.cover, btn);

    btn->setIconSize(QSize(30, 30));
    btn->setToolTip(info.name);
    btn->setCursor(Qt::PointingHandCursor);
    btn->setProperty("playlist", QVariant::fromValue(info));
    btn->setCheckable(true);

    m_group->addButton(btn);

    int number = layout->count() - 1;

    layout->insertWidget(layout->count() - 1, btn);

    // 点击歌单按钮
    connect(btn, &QPushButton::clicked, [this, btn, number](){
        PlayListInfo info = btn->property("playlist").value<PlayListInfo>();
        emit playlistUpdated(DbManager::getInstance().queryPlaylistId(info.id)); // 发射歌单更新信号，传递歌单中的歌曲ID集合
        emit playlistClicked(info); // 发射歌单点击信号，传递歌单信息
        emit pageChanged(Page::PlayList); // 发射页面切换信号，切换到歌单页面
        m_playlistNumber = number;
    });

    btn->setContextMenuPolicy(Qt::CustomContextMenu);

    // 右击歌单按钮显示菜单栏
    connect(btn, &QPushButton::customContextMenuRequested, this, [btn, this, info](const QPoint &pos) {
        m_contextMenu->setProperty("playlist_id", info.id); // 将歌单ID存储在右键菜单的属性中);
        btn->click();
        m_contextMenu->exec(btn->mapToGlobal(pos)); // 显示右键菜单
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
    QLineEdit* lineEdit = m_createSonglistDialog->findChild<QLineEdit*>("lineEdit");
    lineEdit->clear();
}


void UISideBar::rightClickPlaylist(QAction *action)
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
    m_group->removeButton(btn);
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
    case Page::Local: return ui->localBtn;
    case Page::Collect: return ui->collectBtn;
    case Page::NetWork: return ui->networkBtn;
    default: return nullptr;
    }
}

int UISideBar::playlistNumber()
{
    return m_playlistNumber;
}

void UISideBar::updatePlaylistCover(int playlist_id, const QString &path)
{
    QAbstractButton* btn = findPlaylist(playlist_id);
    QPixmap pix = roundPixmap(QPixmap(path), QSize(30, 30), 5);
    btn->setIcon(pix);

    PlayListInfo info = btn->property("playlist").value<PlayListInfo>();
    info.cover = path;
    btn->setProperty("playlist", QVariant::fromValue(info));
}

void UISideBar::setPlaylistCover(const QString &path, QPushButton *btn)
{
    CoverUtils::loadCoverAsync(path, QSize(30, 30), 5,
                               [btn](const QPixmap& pix){btn->setIcon(pix);},
                               [btn](){btn->setIcon(defaultCover());},
                               QVariant::fromValue(btn));
}

void UISideBar::on_addSongBtn_clicked()
{
    // 显示创建歌单对话框
    int result = m_createSonglistDialog->exec();

    // 获取用户输入的歌单名称
    QLineEdit* lineEdit = m_createSonglistDialog->findChild<QLineEdit*>("lineEdit");
    QString name = lineEdit->text();

    // 如果用户点击了接受按钮并且输入不为空，则在数据库中创建新的歌单，并调用createPlayList方法创建对应的歌单按钮
    if(result == QDialog::Accepted && !name.isEmpty()){
        PlayListInfo info = DbManager::getInstance().createPlaylist(1, name);
        createPlaylist(info);
        emit playlistCreated(info);
    }

    // 清空歌单名称输入框的文本，以便下次创建歌单时输入框为空
    lineEdit->clear();
}

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

void UISideBar::on_settingBtn_clicked()
{
    // emit pageChanged(Page::Setting);

    // QNetworkAccessManager *manager = new QNetworkAccessManager(this);

    // QNetworkRequest request(QUrl("http://192.168.85.168:8080/songs"));

    // QNetworkReply *reply = manager->get(request);

    // connect(&reply, &QNetworkReply::finished, this, [this, rely](){

    // });


}














