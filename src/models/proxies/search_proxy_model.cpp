#include "search_proxy_model.h"
#include "global.h"
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

    return title.contains(m_keyword, Qt::CaseInsensitive) ||
           artist.contains(m_keyword, Qt::CaseInsensitive);
}
