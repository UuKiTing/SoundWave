#ifndef SEARCHBAR_DELEGATE_H
#define SEARCHBAR_DELEGATE_H

#include <QStyledItemDelegate>

class SearchBarDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit SearchBarDelegate(QObject *parent = nullptr);

    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

};

#endif // SEARCHBAR_DELEGATE_H
