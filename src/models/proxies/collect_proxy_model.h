#ifndef COLLECT_PROXY_MODEL_H
#define COLLECT_PROXY_MODEL_H

#include <QSortFilterProxyModel>
#include <QSet>

class CollectProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit CollectProxyModel(QObject *parent = nullptr);

    void setSourceModel(QAbstractItemModel *sourceModel) override;

protected:

    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

private:
    void flushVisibleRows();

    QSet<int> m_visiableRows;
};

#endif // COLLECT_PROXY_MODEL_H
