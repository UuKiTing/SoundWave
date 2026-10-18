#ifndef CREATE_PLAYLIST_DIALOG_H
#define CREATE_PLAYLIST_DIALOG_H

#include "playlist_info.h"
#include <QDialog>

namespace Ui {
class UICreatePlaylistDialog;
}

class CreatePlaylistDialog : public QDialog
{
    Q_OBJECT
public:
    CreatePlaylistDialog(QWidget *parent = nullptr);
    ~CreatePlaylistDialog();

    void addPlaylist();

signals:
    PlayListInfo playlistAdded(const QString &name);
    void playlistCreated(const PlayListInfo &info);

private:
    connectSignals();

    Ui::UICreatePlaylistDialog *ui;
};

#endif // CREATE_PLAYLIST_DIALOG_H
