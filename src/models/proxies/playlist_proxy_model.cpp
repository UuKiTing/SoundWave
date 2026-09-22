#include "playlist_proxy_model.h"
#include "song_info.h"
#include "model_roles.h"

PlayListProxyModel::PlayListProxyModel(QObject *parent)
    : QSortFilterProxyModel{parent}
{}

void PlayListProxyModel::setAllowedSongIds(const QSet<int> &songIds)
{
    m_allowedSongIds = songIds;
    m_visibleRows.clear();

    for(int row = 0; row < sourceModel()->rowCount(); ++row){
        int id = sourceModel()->index(row, 0).data(Roles::Id).toInt();

        if(m_allowedSongIds.contains(id)){
            m_visibleRows.insert(row);
            m_allowedSongIds.remove(id);
        }
    }

    invalidateFilter();
}

bool PlayListProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    if(m_visibleRows.isEmpty()) return false;

    return m_visibleRows.contains(source_row);
}
