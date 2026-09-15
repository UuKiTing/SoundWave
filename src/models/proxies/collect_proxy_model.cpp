#include "collect_proxy_model.h"
#include "global.h"
#include <QAbstractItemModel>

CollectProxyModel::CollectProxyModel(QObject *parent)
    : QSortFilterProxyModel{parent}
{}

bool CollectProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    QAbstractItemModel *source = sourceModel();

    return  source->index(source_row, 0).data(Roles::IsFavorite).toBool();
}
