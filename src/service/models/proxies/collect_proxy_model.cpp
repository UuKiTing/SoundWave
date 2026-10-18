#include "collect_proxy_model.h"
#include "model_roles.h"
#include "song_info.h"
#include <QAbstractItemModel>

CollectProxyModel::CollectProxyModel(QObject *parent)
    : QSortFilterProxyModel{parent}
{}

void CollectProxyModel::setSourceModel(QAbstractItemModel *sourceModel)
{
    QSortFilterProxyModel::setSourceModel(sourceModel);

    connect(sourceModel, &QAbstractItemModel::modelReset, this, &CollectProxyModel::flushVisibleRows);
    connect(sourceModel, &QAbstractItemModel::dataChanged, this, [this](const QModelIndex &topLeft, const QModelIndex &bottomRight,
                                                                        const QList<int> &roles = QList<int>()){
        if (!roles.isEmpty()
            && !roles.contains(Roles::IsFavorite)
            && !roles.contains(Roles::Id)
            && !roles.contains(Roles::IsValid)) {
            return;
        }

        flushVisibleRows();
    });

    connect(sourceModel, &QAbstractItemModel::rowsInserted, this, &CollectProxyModel::flushVisibleRows);
    connect(sourceModel, &QAbstractItemModel::rowsRemoved, this, &CollectProxyModel::flushVisibleRows);

    flushVisibleRows();
}

bool CollectProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    const QModelIndex index = sourceModel()->index(source_row, 0);

    if(!index.data(Roles::IsFavorite).toBool()){
        return false;
    }

    const int id = index.data(Roles::Id).toInt();

    return m_songRows.value(id, -1) == source_row;
}

void CollectProxyModel::flushVisibleRows()
{
    m_songRows.clear();

    for(int row = 0; row < sourceModel()->rowCount(); ++row){
        QModelIndex index = sourceModel()->index(row, 0);

        if(!index.data(Roles::IsFavorite).toBool())
            continue;

        const bool isLocal = index.data(Roles::Source).value<SongSource>() == SongSource::Local;

        if(isLocal && !index.data(Roles::IsValid).toBool())
            continue;

        const int id = index.data(Roles::Id).toInt();
        auto it = m_songRows.find(id);

        if(it == m_songRows.end()){
            m_songRows.insert(id, row);
            continue;
        }
        else{
            it.value() = row;
        }

        const QModelIndex previousIndex = sourceModel()->index(it.value(), 0);
        const bool previousIsLocal = previousIndex.data(Roles::Source).value<SongSource>() == SongSource::Local;

        if((isLocal && !previousIsLocal)){
            it.value() = row;
        }
    }

    invalidateFilter();
}
