#ifndef CAROUSEL_LABEL_H
#define CAROUSEL_LABEL_H

#include <QLabel>
#include <QPen>
#include <QFont>
#include <QTimer>
#include <QTextDocument>

enum Direction{
    Left,
    Right
};

// 文本标签轮播类
class CarouselLabel : public QLabel
{
    Q_OBJECT
public:
    explicit CarouselLabel(QWidget *parent = nullptr);

    // setText 设置文本
    void setText(const QString&);

    // isTextOverflow 判断文本是否超出标签范围
    bool isTextOverflow();

    // setDirection 设置轮播的方向
    void setDirection(const Direction &direction);

    // setSpeed 设置轮播速度
    void setSpeed(int speed);

    // setInterval 设置计时器周期速度
    void setInterval(int msec);

    // setRichTextStyle 设置富文本样式
    void setRichTextStyle();

protected:
    void paintEvent(QPaintEvent*) override;
    void changeEvent(QEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    void onTimerTimeOut();

    // resetPos 重置文本位置
    void resetPos(int &offset, bool &showFlag);

    // shouldShow 判断文本是否应该显示
    bool shouldShow(int offset);

    // updateTextWidth 更新文本宽度
    void updateTextWidth(const QString &);

    QTimer *m_timer;

    Direction m_direction = Direction::Left; ///< 轮播方向

    QTextDocument m_doc;

    int m_offset = 0;
    int m_offset2 = 0;
    int m_textWidth = 0;
    int m_speed = 1;

    bool m_isRichText = false; // 是否为富文本
    bool m_isHovered = false; //  鼠标是否

    bool m_beforeShow = true;
    bool m_afterShow = false;

    int m_msec = 30; // 轮播时间周期
};

#endif // CAROUSEL_LABEL_H
