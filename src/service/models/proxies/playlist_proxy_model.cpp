#include "playlist_proxy_model.h"
#include "song_info.h"
#include "model_roles.h"

PlayListProxyModel::PlayListProxyModel(QObject *parent)
    : QSortFilterProxyModel{parent}
{}

void PlayListProxyModel::setAllowedSongIds(const QSet<int> &songIds)
{
    m_allowedSongIds = songIds;
    m_songRows.clear();

    for(int row = 0; row < sourceModel()->rowCount(); ++row){
        const QModelIndex index = sourceModel()->index(row, 0);
        const int id = index.data(Roles::Id).toInt();
        const bool isLocal = index.data(Roles::Source).value<SongSource>() == SongSource::Local;

        if(m_allowedSongIds.contains(id)){

            if(isLocal && !index.data(Roles::IsValid).toBool())
                continue;

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

            if(isLocal && !previousIsLocal){
                it.value() = row;
            }
        }
    }

    invalidateFilter();
}

bool PlayListProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    if(m_songRows.isEmpty()) return false;

    const QModelIndex index = sourceModel()->index(source_row, 0);
    const int id = index.data(Roles::Id).toInt();

    return m_songRows.value(id, -1) == source_row;

}
