#ifndef STYLEITEM_DELEGATE_H
#define STYLEITEM_DELEGATE_H

#include <QObject>
#include <QStyledItemDelegate>
#include <QCache>
#include <QPixmap>
#include <QFutureWatcher>

class ImageLoader;
class ImageTask;

class StyleItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit StyleItemDelegate(QObject *parent = nullptr);

    static QRect iconRectFor(const QRect &r, int iconSize);
    static QRect durationRectFor(const QRect &r, int durWidth, int durMarginRight);
    static QRect favBtnRectFor(const QRect &r, int btnSize, int btnMarginRight);
    static void textRectsFor(QRect &titleRect, QRect &artistRect, const QRect &r,int marginLeft, int width);

    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

signals:
    void collected(const QModelIndex &index);
    void cancelCollected(const QModelIndex &index);

protected:
    virtual bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;

private:
    const int ICON_SIZE = 50;
    const int ICON_RADIUS = 5;
    const int DUR_MARGIN_RIGHT = 50;
    const int DUR_WIDTH = 50;
    const int BUTTON_SIZE  = 20;
    const int BUTTON_MARGIN_RIGHT = 25 + DUR_WIDTH + DUR_MARGIN_RIGHT;
    const int TEXT_MARGIN_LEFT = 15;

    ImageLoader* m_loader;
    QThread* m_thread;
};

#endif // STYLEITEM_DELEGATE_H
