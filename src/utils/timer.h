#ifndef TIMER_H
#define TIMER_H

#include <QObject>
#include <QTimer>
#include <functional>

class Timer : public QTimer
{
    Q_OBJECT
public:
    explicit Timer(QObject *parent = nullptr);

    void createTimer(int msec, bool singleShot, std::function<bool()> func = nullptr);

    static QString timeToString(int msec);

signals:


};

#endif // TIMER_H
