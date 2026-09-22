#ifndef URL_CONFIG_H
#define URL_CONFIG_H

#include <QString>
#include <QFile>

inline QString BaseUrl = "http://192.168.85.128:8080";
inline QString SongsJsonUrl = QString(BaseUrl) + "/songsJson";
inline QString SongAudioUrl = QString(BaseUrl) + "/songAudio/";
inline QString SongImageUrl = QString(BaseUrl) + "/songImage/";
inline QString SongLyricsUrl = QString(BaseUrl) + "/songLyrics/";


#endif // URL_CONFIG_H


