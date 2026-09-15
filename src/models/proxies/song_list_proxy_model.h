#ifndef SONG_LIST_PROXY_MODEL_H
#define SONG_LIST_PROXY_MODEL_H

#include <QObject>
#include <QSortFilterProxyModel>

class PlayListProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit PlayListProxyModel(QObject *parent = nullptr);

    void setAllowedSongIds(const QSet<int> &songIds);
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;

private:
    QSet<int> m_allowedSongIds{};
};

#endif // SONG_LIST_PROXY_MODEL_H
