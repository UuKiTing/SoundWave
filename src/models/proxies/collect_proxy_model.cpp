#include "collect_proxy_model.h"
#include "global.h"
#include <QAbstractItemModel>

CollectProxyModel::CollectProxyModel(QObject *parent)
    : QSortFilterProxyModel{parent}
{}

void CollectProxyModel::setSourceModel(QAbstractItemModel *sourceModel)
{
    QSortFilterProxyModel::setSourceModel(sourceModel);

    connect(sourceModel, &QAbstractItemModel::modelReset, this, &CollectProxyModel::flushVisibleRows);
    connect(sourceModel, &QAbstractItemModel::dataChanged, this, &CollectProxyModel::flushVisibleRows);
    connect(sourceModel, &QAbstractItemModel::rowsInserted, this, &CollectProxyModel::flushVisibleRows);
    connect(sourceModel, &QAbstractItemModel::rowsRemoved, this, &CollectProxyModel::flushVisibleRows);

    flushVisibleRows();
}

bool CollectProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    return m_visiableRows.contains(source_row);
}

void CollectProxyModel::flushVisibleRows()
{
    m_visiableRows.clear();

    QSet<int> set;

    for(int i = 0; i < sourceModel()->rowCount(); ++i){
        QModelIndex index = sourceModel()->index(i, 0);

        if(!index.data(Roles::IsFavorite).toBool()) continue;

        int id = index.data(Roles::Id).toInt();

        if(set.contains(id)) continue;

        set.insert(id);

        m_visiableRows.insert(i);
    }

    invalidateFilter();
}




















