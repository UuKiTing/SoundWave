#ifndef SOUNDWAVE_H
#define SOUNDWAVE_H

#include "player_controller.h"
#include "song_manager.h"
#include "mainwindow.h"
#include "appmediator.h"
#include "config_manager.h"
#include <QWidget>
#include <QMoveEvent>
#include <QResizeEvent>
#include <QSystemTrayIcon>
#include <QMenu>

class SoundWave : public QWidget
{
    Q_OBJECT

public:
    explicit SoundWave(QWidget *parent = nullptr);
    ~SoundWave();

    // 显示主窗口
    void show();

    // 绑定视图与模型
    void bindViewAndModel(UIMain *uiMain, UISearch *uiSearch, SongManager *manager);

    // 是否初始化成功
    bool isInitialized();

private:
    // 加载用户状态
    void loadUserState();

    // 记录用户状态
    void recordUserState();

    AppMediator *m_mediator{}; // 应用中介者
    PlayerController *m_controller{}; // 播放控制器
    SongManager *m_songManager{}; // 列表管理器
    MainWindow *m_mainWindow{}; // 主窗口
    ConfigManager *m_configManager{}; // 配置管理器

    bool m_initalized = false; // 是否初始化完成
};

#endif // SOUNDWAVE_H
