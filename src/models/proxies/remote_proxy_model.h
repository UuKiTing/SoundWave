#ifndef REMOTE_PROXY_MODEL_H
#define REMOTE_PROXY_MODEL_H

#include <QSortFilterProxyModel>

class RemoteProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit RemoteProxyModel(QObject *parent = nullptr);\


protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
};

#endif // REMOTE_PROXY_MODEL_H
