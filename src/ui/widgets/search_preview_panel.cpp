#include "search_preview_panel.h"
#include "ui_search_preview_panel.h"
#include "searchbar_delegate.h"

SearchPreviewPanel::SearchPreviewPanel(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SearchPreviewPanel)
{
    ui->setupUi(this);

    SearchBarDelegate *delegate = new SearchBarDelegate(this);

    ui->searchListView->setItemDelegate(delegate);

    ui->searchListView->setEditTriggers(QAbstractItemView::EditTrigger::NoEditTriggers);;

    ui->searchListView->setStyleSheet(R"(
    QListView {
        background-color: white;
        border: none;
        outline: none;
    }
    QListView::item {
        height: 48px;
        padding: 6px 10px;
        font-size: 14px;
        color: #333333;
        border-bottom: 1px solid #f0f0f0;
    }

)");
}

SearchPreviewPanel::~SearchPreviewPanel()
{
    delete ui;
}

void SearchPreviewPanel::switchStackedWidget(int pageIndex)
{
    ui->stackedWidget->setCurrentIndex(pageIndex);
}

QListView *SearchPreviewPanel::searchListView()
{
    return ui->searchListView;
}
