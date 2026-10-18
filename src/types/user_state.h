#ifndef USER_STATE_H
#define USER_STATE_H

#include "play_mode.h"
#include "page.h"

class UserState{
public:
    void setCurrentPlaylist(int playlist_id);
    int currentPlaylist();

    void setPlayPage(MainPage page);
    MainPage playPage();

    void setCurrentRow(int row);
    int currentRow();

    void setVolume(float volume);
    int volume();

    void setPosition(int position);
    int position();

    void setPlayMode(PlayMode mode);
    PlayMode playMode();

private:
    int m_currentPlaylist = -1;
    MainPage m_playPage = MainPage::Local;
    int m_currentRow = -1;
    int m_volume = 20;
    int m_position = 0;
    PlayMode m_playMode = PlayMode::Loop;
};

#endif // USER_STATE_H
