#include "create_playlist_dialog.h"
#include "ui_create_playlist_dialog.h"
#include "contextmenu.h"

CreatePlaylistDialog::CreatePlaylistDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::UICreatePlaylistDialog)
{
    ui->setupUi(this);

    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog); // 设置无边框和对话框属性
    setAttribute(Qt::WA_TranslucentBackground); // 设置背景透明

    connectSignals();
}

CreatePlaylistDialog::~CreatePlaylistDialog()
{
    delete ui;
}

void CreatePlaylistDialog::addPlaylist()
{
    QString name = ui->lineEdit->text();

    if(!name.isEmpty()){
        PlayListInfo info = emit playlistAdded(name);
        ContextMenu::getInstance().addPlaylist(info);
        emit playlistCreated(info);
    }

    ui->lineEdit->clear();
}

CreatePlaylistDialog::connectSignals()
{
    connect(ui->acceptBtn, &QPushButton::clicked, this, &QDialog::accept);
    connect(ui->cancelBtn, &QPushButton::clicked, this, &QDialog::reject);

    connect(ui->cancelBtn, &QPushButton::clicked, ui->lineEdit, &QLineEdit::clear);
}
