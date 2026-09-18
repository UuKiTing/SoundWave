#include "remote_proxy_model.h"
#include "global.h"

RemoteProxyModel::RemoteProxyModel(QObject *parent)
    : QSortFilterProxyModel{parent}
{}

bool RemoteProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    QAbstractItemModel *source = sourceModel();

    return  source->index(source_row, 0).data(Roles::Source).value<SongSource>() == SongSource::Remote;
}
