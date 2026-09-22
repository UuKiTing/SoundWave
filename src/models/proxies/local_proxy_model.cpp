#include "local_proxy_model.h"
#include "model_roles.h"
#include "song_info.h"

LocalProxyModel::LocalProxyModel(QObject *parent)
    : QSortFilterProxyModel{parent}
{}

bool LocalProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    QAbstractItemModel *source = sourceModel();

    SongSource songSource = source->index(source_row, 0).data(Roles::Source).value<SongSource>();
    bool isValid = source->index(source_row, 0).data(Roles::IsValid).toBool();

    return  (songSource == SongSource::Local && isValid);
}
