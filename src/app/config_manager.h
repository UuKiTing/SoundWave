#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include "user_state.h"
#include <QObject>

class ConfigManager : public QObject
{
    Q_OBJECT

public:
    explicit ConfigManager(QObject *parent = nullptr);

    void saveSetting(); // 保存配置
    void loadSetting(); // 加载配置

    void setVolume(float volume); // 保存音量大小
    void setPlayPage(MainPage page); // 保存当前播放页面类型
    void setCurrentRow(int row); // 保存当前播放行
    void setPlayMode(PlayMode playMode); // 保存当前播放模式
    void setPosition(int position); // 保存当前播放进度
    void setCurrentPlaylist(int playlist_id); // 保存当前播放的歌单

signals:
    void volumeChanged(int volume);
    void pageChanged(MainPage page, int playlist_id);
    void currentRowChanged(int row);
    void playmodeChanged(PlayMode playMode);
    void positionChanged(int position);

private:
    UserState m_userState;
};

#endif // CONFIG_MANAGER_H
