#ifndef LOCAL_PROXY_MODEL_H
#define LOCAL_PROXY_MODEL_H

#include <QSortFilterProxyModel>

class LocalProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit LocalProxyModel(QObject *parent = nullptr);

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
};

#endif // LOCAL_PROXY_MODEL_H
