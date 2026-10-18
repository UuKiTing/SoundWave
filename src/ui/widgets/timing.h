#ifndef TIMING_H
#define TIMING_H

#include "timer.h"
#include <QDialog>
#include <QButtonGroup>

namespace Ui {
class UITiming;
}

class Timing : public QDialog
{
    Q_OBJECT
public:
    explicit Timing(QWidget *parent = nullptr);
    ~Timing();

    bool startCountdown();

    void stop();

    bool update();

signals:
    void secondElapsed(const QString &timeText);
    void timerStarted();
    void timeout();


private:
    void init();

    void connectSignals();

    Ui::UITiming *ui;

    Timer *m_mainTimer;
    Timer *m_updateTimer;

    QButtonGroup *m_buttonGroup;
};

#endif // TIMING_H
