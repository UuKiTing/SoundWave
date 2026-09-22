#include "remote_proxy_model.h"
#include "model_roles.h"
#include "song_info.h"
#include <QThread>
#include <QApplication>

RemoteProxyModel::RemoteProxyModel(QObject *parent)
    : QSortFilterProxyModel{parent}
{}

bool RemoteProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    QAbstractItemModel *source = sourceModel();

    return  source->index(source_row, 0).data(Roles::Source).value<SongSource>() == SongSource::Remote;
}
