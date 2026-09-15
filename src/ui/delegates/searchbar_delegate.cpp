#include "searchbar_delegate.h"
#include "global.h"
#include <QPainter>

SearchBarDelegate::SearchBarDelegate(QObject *parent)
    : QStyledItemDelegate{parent}
{

}

void SearchBarDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    const QString title = index.data(Roles::Title).toString();
    const QString artist = index.data(Roles::Artist).toString();

    painter->save();
    if (option.state & QStyle::State_Selected) {
        painter->fillRect(option.rect, Qt::white);
    } else if (option.state & QStyle::State_MouseOver) {
        painter->fillRect(option.rect, QColor(0xf1f1f1));
    } else {
        painter->fillRect(option.rect, Qt::white);
    }
    painter->restore();

    painter->save();
    QFont font = painter->font();
    font.setPixelSize(12);
    painter->setFont(font);
    painter->setPen(Qt::black);
    painter->drawText(15, option.rect.top() + 20, title + "-" + artist);
    painter->restore();

    QStyledItemDelegate::paint(painter, option, index);
}

QSize SearchBarDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QSize(option.rect.width(), 40);

}
