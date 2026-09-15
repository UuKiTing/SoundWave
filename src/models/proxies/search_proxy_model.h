#ifndef SEARCH_PROXY_MODEL_H
#define SEARCH_PROXY_MODEL_H

#include <QSortFilterProxyModel>


class SearchProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit SearchProxyModel(QObject *parent = nullptr);

    void setKeyWord(const QString &keyword);

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

private:
    QString m_keyword;

};

#endif // SEARCH_PROXY_MODEL_H
