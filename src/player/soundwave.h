#ifndef SOUNDWAVE_H
#define SOUNDWAVE_H

#include "player_controller.h"
#include "song_manager.h"
#include "uimain.h"
#include "uisidebar.h"
#include "uisearch.h"
#include "appmediator.h"
#include "music_detail_widget.h"
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

    bool initialize();  ///< 初始化

protected:
    /**  @brief 程序窗口关闭事件 */
    void closeEvent(QCloseEvent *event) override;

    /**  @brief 程序窗口更改 */
    void resizeEvent(QResizeEvent *event) override;

    /**  @brief 快捷键 */
    void keyPressEvent(QKeyEvent *event) override;

private:
    /**  @brief 初始化布局 */
    void initLayout();

    /**  @brief 保存配置 */
    void saveSettings();

    /**  @brief 加载配置 */
    void loadSettings();

    /**  @brief 创建系统托盘 */
    bool createTrayIcon();

    void viewBindModel(UIMain *uiMain, UISearch *uiSearch, SongManager *manager);

    AppMediator *m_mediator{}; ///< 应用中介者
    PlayerController *m_controller{}; ///< 播放控制器
    SongManager *m_songManager{}; ///< 列表管理器
    UIMain *m_uiMain{}; ///< 主界面
    UISideBar *m_uiSideBar{}; ///< 侧边栏
    UISearch *m_uiSearch{}; ///< 搜索栏
    MusicDetailWidget *m_detailWidget{}; ///< 歌曲详情页
    QSystemTrayIcon *m_trayIcon{};  ///< 系统托盘图标
    QMenu *m_trayMenu{}; ///< 托盘菜单栏

    bool m_isQuitting = false; ///< 是否退出

    bool m_isLoadedSetting = false;
};

#endif // SOUNDWAVE_H
