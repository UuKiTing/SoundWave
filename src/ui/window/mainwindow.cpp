#include "mainwindow.h"
#include "dbmanager.h"
#include "song_manager.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QWidget{parent}
{
    m_uiMain = new UIMain(this);
    m_uiSideBar = new UISideBar(this);
    m_uiSearch = new UISearch(this);
    m_detailWidget = new MusicDetailWidget(m_uiMain);
}

bool MainWindow::initialize()
{
    this->setWindowIcon(QIcon(":/icon/icon.png"));

    initLayout();

    if(!createTrayIcon()){
        qDebug() << "创建系统托盘失败！";
        return false;
    }

    m_trayIcon->show();

    return true;
}

void MainWindow::initLayout()
{
    QHBoxLayout *hBox = new QHBoxLayout(this);
    hBox->setContentsMargins(0, 0, 0, 0);
    hBox->addWidget(m_uiSideBar);
    hBox->setSpacing(0);

    QVBoxLayout *vBox = new QVBoxLayout;
    vBox->setContentsMargins(0, 0, 0, 0);
    vBox->setSpacing(0);
    vBox->addWidget(m_uiSearch);
    vBox->addWidget(m_uiMain);

    hBox->addLayout(vBox);

    m_detailWidget->hide();
    m_detailWidget->stackUnder(m_uiMain->controlBar()); // 将主界面控制栏置于全屏页上面
}

UIMain *MainWindow::uiMain()
{
    return m_uiMain;
}

UISideBar *MainWindow::uiSideBar()
{
    return m_uiSideBar;
}

UISearch *MainWindow::uiSearch()
{
    return m_uiSearch;
}

MusicDetailWidget *MainWindow::detailWidget()
{
    return m_detailWidget;
}

bool MainWindow::createTrayIcon()
{
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        return false;
    }

    m_trayIcon = new QSystemTrayIcon(this);
    m_trayIcon->setIcon(QIcon(":/icon/icon.png"));
    m_trayIcon->setToolTip(tr("我的应用"));

    // 创建托盘菜单
    m_trayMenu = new QMenu(this);

    QAction *quitAction = new QAction(QIcon(":/icon/quit.png"), tr("退出"), this);

    connect(quitAction, &QAction::triggered, [this](){
        m_isQuitting = true;
        this->close();
    });

    connect(m_trayIcon, &QSystemTrayIcon::activated, [this](QSystemTrayIcon::ActivationReason reason) {
        // 判断触发的原因是否为“双击”
        if (reason == QSystemTrayIcon::DoubleClick) {
            // 判断是否最小化
            if (this->isMinimized()) {
                this->showNormal();
            } else {
                this->show();
            }
            this->activateWindow(); // 将窗口置顶并获取焦点
        }
    });

    m_trayMenu->addAction(quitAction);

    m_trayIcon->setContextMenu(m_trayMenu);

    return true;
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    if (!m_isQuitting && m_trayIcon && m_trayIcon->isVisible()) {
        hide();  // 隐藏
        event->ignore();  // 忽略关闭事件
        return;
    }

    emit settingsSaved();

    event->accept();
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    // 当窗口大小改变时，调整音乐详情页的大小和位置
    if(m_detailWidget){
        m_detailWidget->setGeometry(0, 0, this->width(), this->height());
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Space){
        // m_controller->playOrPause();
    }
    else if(event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_Left){
        emit m_uiMain->songSkipped(false);
    }
    else if(event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_Right){
        emit m_uiMain->songSkipped(true);
    }

    QWidget::keyPressEvent(event);
}
