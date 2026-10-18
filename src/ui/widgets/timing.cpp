#include "timing.h"
#include "ui_timing.h"
#include <QMessageBox>
#include <functional>

Timing::Timing(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::UITiming)
{
    ui->setupUi(this);

    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog); // 设置无边框和对话框属性
    setAttribute(Qt::WA_TranslucentBackground); // 设置背景透明

    m_buttonGroup = new QButtonGroup(this);

    init();

    connectSignals();
}

Timing::~Timing()
{
    delete ui;
}

void Timing::init()
{
    QList<QCheckBox*> list = ui->options->findChildren<QCheckBox*>();

    for (QCheckBox *checkBox : list) {
        int minutes = 0;

        if(checkBox->text().contains("分钟")){
            minutes = checkBox->text().left(2).toInt();
        };

        m_buttonGroup->addButton(checkBox, minutes);
    }

    ui->minuteSpinBox->hide();
    ui->hourSpinBox->hide();

    ui->options->setDisabled(true);

    connect(ui->minute, &QCheckBox::toggled, ui->minuteSpinBox, &QWidget::setVisible);
    connect(ui->minute, &QCheckBox::toggled, ui->hourSpinBox, &QWidget::setVisible);
    connect(ui->timeSwitch, &QCheckBox::toggled, this, [this](bool checked)
            {ui->options->setDisabled(!checked);
    });

    m_mainTimer = new Timer(this);
    m_updateTimer = new Timer(this);
}

void Timing::connectSignals()
{
    connect(ui->acceptBtn, &QPushButton::clicked, this, &QDialog::accept);
    connect(ui->cancelbtn, &QPushButton::clicked, this, &QDialog::reject);
    connect(m_mainTimer, &Timer::timeout, this, &Timing::timeout);
    connect(m_mainTimer, &Timer::timerStarted, this, &Timing::timerStarted);
}

bool Timing::startCountdown()
{
    if(!ui->timeSwitch->isChecked()) return true;

    int sec = 0;

    if(m_buttonGroup->checkedId() > 0){
        sec = m_buttonGroup->checkedId() * 60;
    }
    else{
        sec =  ui->defineHour->value() * 60 + ui->defineMinute->value() * 60;
    }

    if(sec == 0){
        QMessageBox::warning(this, "警告⚠️", "不能为空");
        return false;
    }

    m_mainTimer->createTimer(sec * 1000, true, [this](){
        // emit songPlayedOrPaused();
        return false;
    });

    m_updateTimer->createTimer(1000, false, std::bind(&Timing::update, this));

    return true;
}

void Timing::stop()
{
    m_mainTimer->stop();
    m_updateTimer->stop();
}

bool Timing::update()
{
    int msec = m_mainTimer->remainingTime();
    if(msec > 0) {
        emit secondElapsed(Timer::timeToString(msec));
        return true;
    }
    return false;
}
