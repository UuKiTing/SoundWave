#include "timer.h"
#include <QTime>

Timer::Timer(QObject *parent)
    : QTimer{parent}
{}

void Timer::createTimer(int msec, bool singleShot, std::function<bool()> func)
{
    this->setSingleShot(singleShot);

    disconnect(this, &Timer::timeout, nullptr, nullptr);

    if(func){
        connect(this, &Timer::timeout, this, [this, func](){
            bool keep = func();
            if(!keep){
                this->stop();
            }
        });
    }

    this->start(msec);
}

QString Timer::timeToString(int msec)
{
    if(msec <= 0) return "00:00:00";

    return QTime(0, 0, 0).addMSecs(msec).toString("hh:mm:ss");
}
