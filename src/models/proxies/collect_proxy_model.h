#ifndef COLLECT_PROXY_MODEL_H
#define COLLECT_PROXY_MODEL_H

#include <QSortFilterProxyModel>

class CollectProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit CollectProxyModel(QObject *parent = nullptr);


protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
};

#endif // COLLECT_PROXY_MODEL_H
