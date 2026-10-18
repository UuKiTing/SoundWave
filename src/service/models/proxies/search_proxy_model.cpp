#include "search_proxy_model.h"
#include "model_roles.h"
#include "song_info.h"
#include <QAbstractItemModel>


SearchProxyModel::SearchProxyModel(QObject *parent)
    : QSortFilterProxyModel{parent}
{

}

void SearchProxyModel::setKeyWord(const QString &keyword)
{
    m_keyword = keyword;
    invalidateFilter();
}

bool SearchProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    if(m_keyword.isEmpty()) return true;

    QAbstractItemModel *source = sourceModel();

    QModelIndex index = source->index(source_row, 0, source_parent);

    QString title = index.data(Roles::Title).toString();
    QString artist = index.data(Roles::Artist).toString();
    SongSource songSource = index.data(Roles::Source).value<SongSource>();

    return title.contains(m_keyword, Qt::CaseInsensitive) || artist.contains(m_keyword, Qt::CaseInsensitive);
}
