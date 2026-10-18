#ifndef URL_CONFIG_H
#define URL_CONFIG_H

#include <QString>
#include <QFile>

namespace Url{
    inline QString BaseUrl = "http://192.168.85.128:8080";

    inline QString songsJsonUrl()  { return BaseUrl + "/songsJson"; }
    inline QString songAudioUrl()  { return BaseUrl + "/songAudio/"; }
    inline QString songImageUrl()  { return BaseUrl + "/songImage/"; }
    inline QString songLyricsUrl() { return BaseUrl + "/songLyrics/"; }

} // namesapce Url


#endif // URL_CONFIG_H


