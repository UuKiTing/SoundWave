#ifndef SEARCH_PREVIEW_PANEL_H
#define SEARCH_PREVIEW_PANEL_H

#include <QWidget>
#include <QListView>

namespace Ui {
class SearchPreviewPanel;
}

class SearchPreviewPanel : public QWidget
{
    Q_OBJECT

public:
    explicit SearchPreviewPanel(QWidget *parent = nullptr);
    ~SearchPreviewPanel();

    void switchStackedWidget(int pageIndex);

    QListView* searchListView();

private:
    Ui::SearchPreviewPanel *ui;
};

#endif // SEARCH_PREVIEW_PANEL_H
