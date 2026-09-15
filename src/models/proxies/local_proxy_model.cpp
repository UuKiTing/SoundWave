#include "local_proxy_model.h"
#include "global.h"

LocalProxyModel::LocalProxyModel(QObject *parent)
    : QSortFilterProxyModel{parent}
{}

bool LocalProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    QAbstractItemModel *source = sourceModel();

    return  source->index(source_row, 0).data(Roles::Source).value<SongSource>() == SongSource::Local;
}
