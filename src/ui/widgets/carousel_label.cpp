#include "carousel_label.h"
#include <QPainter>
#include <QFontMetrics>
#include <QTextDocument>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QStylePainter>
#include <QStyleOption>
#include <QEvent>

CarouselLabel::CarouselLabel(QWidget *parent)
    :QLabel{parent}
{
    m_timer = new QTimer(this);

    connect(m_timer, &QTimer::timeout, this, &CarouselLabel::onTimerTimeOut);
}

void CarouselLabel::setText(const QString &text)
{
    QLabel::setText(text);

    m_isRichText = Qt::mightBeRichText(text);

    setRichTextStyle();

    updateTextWidth(text);

    if(text.isEmpty() || !isTextOverflow())
        m_timer->stop();
    else
        m_timer->start(m_msec);

    m_offset = 0;
    m_offset2 = m_textWidth;
}

bool CarouselLabel::isTextOverflow()
{
    return m_textWidth > width();
}

void CarouselLabel::setDirection(const Direction &direction)
{
    m_direction = direction;
}

void CarouselLabel::setSpeed(int speed)
{
    m_speed = speed;
}

void CarouselLabel::setInterval(int msec)
{
    m_msec = msec;
    if(m_timer->isActive()){
        m_timer->stop();
        m_timer->start(m_msec);
    }
}

void CarouselLabel::setRichTextStyle()
{
    if (m_isRichText) {
        m_doc.setDefaultFont(font());
        m_doc.setHtml(text());
        m_doc.setDocumentMargin(0);
        m_doc.setTextWidth(-1);
    }
}

void CarouselLabel::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QRectF rectf = rect();

    QStyleOption opt;
    opt.initFrom(this);

    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

    if(!isTextOverflow() || m_isHovered){
        if(m_isRichText){
            qreal y = (height() - m_doc.size().height()) / 2.0;

            painter.translate(0, y);
            m_doc.drawContents(&painter);
        }
        else{
            painter.drawText(rectf, Qt::AlignVCenter, text());
        }

        return;
    }


    auto drawText = [&painter, this, &rectf](bool showFlag, int offset){
        if(!showFlag) return;

        painter.save();

        if(m_isRichText){
            qreal y = (height() - m_doc.size().height()) / 2.0;

            painter.translate(offset, y);
            m_doc.drawContents(&painter);
        }
        else{
            painter.setFont(font());
            painter.setPen(palette().color(foregroundRole()));

            rectf.setLeft(offset);

            painter.drawText(rectf, Qt::AlignVCenter, text());
        }

        painter.restore();

    };

    drawText(m_beforeShow, m_offset);
    drawText(m_afterShow, m_offset2);
}

void CarouselLabel::changeEvent(QEvent *event)
{
    QLabel::changeEvent(event);

    if(event->type() == QEvent::FontChange || event->type() == QEvent::StyleChange){
        updateTextWidth(text());
        setRichTextStyle();
        update();
    }
}

void CarouselLabel::enterEvent(QEnterEvent *event)
{
    QLabel::enterEvent(event);
    m_isHovered = true;
    m_timer->stop();
    update();
    setCursor(Qt::PointingHandCursor);
}

void CarouselLabel::leaveEvent(QEvent *event)
{
    QLabel::leaveEvent(event);

    m_isHovered = false;
    if(isTextOverflow()){
        m_timer->start(m_msec);
    }
    unsetCursor();
    update();
}

void CarouselLabel::onTimerTimeOut()
{
    int d = (m_direction == Direction::Left) ? -1 : 1;

    if(m_beforeShow) m_offset += d * m_speed;
    if(m_afterShow) m_offset2 += d * m_speed;

    resetPos(m_offset, m_beforeShow);
    resetPos(m_offset2, m_afterShow);

    if(shouldShow(m_offset)) m_afterShow = true;
    if(shouldShow(m_offset2)) m_beforeShow = true;

    update();
}

void CarouselLabel::resetPos(int &offset, bool &showFlag)
{
    if(m_direction == Direction::Left){
        if(offset <= -m_textWidth){
            showFlag = false;
            offset = width();
        }
    }
    else{
        if(offset >= width()){
            showFlag = false;
            offset = -m_textWidth;
        }
    }
}

bool CarouselLabel::shouldShow(int offset)
{
    if(m_direction == Direction::Left){
        return offset < 0 && m_textWidth + offset < width() / 2;
    }
    else{
        return offset > width() / 2;
    }
}

void CarouselLabel::updateTextWidth(const QString &text)
{
    if(m_isRichText){
        m_textWidth = qCeil(m_doc.idealWidth());
    }
    else{
        QFontMetrics fontMetrics(font());
        m_textWidth = fontMetrics.horizontalAdvance(text);
    }
}
