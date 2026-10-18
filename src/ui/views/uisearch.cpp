#include "page.h"
#include "uisearch.h"
#include "ui_uisearch.h"
#include "contextmenu.h"
#include <QTimer>
#include <QMouseEvent>
#include <QApplication>
#include <QSortFilterProxyModel>

UISearch::UISearch(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::UISearch)
{
    ui->setupUi(this);

    ui->searchBar->installEventFilter(this);

    m_previewPanel = new SearchPreviewPanel(this);
    m_previewPanel->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);

    connectSignal();

    window()->installEventFilter(this); // 安装事件过滤器
}

UISearch::~UISearch()
{
    delete ui;
}

void UISearch::connectSignal()
{
    // 搜索列表双击播放事件
    connect(m_previewPanel->searchListView(), &QListView::doubleClicked, this, &UISearch::doubleClickPlay);
}

void UISearch::doubleClickPlay(const QModelIndex &index)
{
    // 延迟隐藏预览面板，避免在双击时立即隐藏导致的界面闪烁
    QTimer::singleShot(100, this, &UISearch::hidePreviewPanel);

    // 发射播放歌曲请求信号
    emit songPlayRequest(index, true);
}

void UISearch::switchStackedWidget(QString text)
{
    if(!text.isEmpty()){
        m_previewPanel->switchStackedWidget(SearchPage::Search);
    }
}

QString UISearch::searchText()
{
    return ui->searchBar->text();
}

QAbstractItemView *UISearch::searchListView()
{
    return m_previewPanel->searchListView();
}

void UISearch::showPreviewPanel()
{
    QPoint globalPos = ui->searchBar->mapToGlobal(QPoint(0, ui->searchBar->height())); // 计算搜索栏的位置
    m_previewPanel->move(globalPos);
    m_previewPanel->resize(ui->searchBar->width(), 200);
    m_previewPanel->show();
}

void UISearch::hidePreviewPanel()
{
    m_previewPanel->hide();
    ui->searchBar->clearFocus();
}


void UISearch::setModel(QAbstractItemView *view, QAbstractItemModel *model)
{
    view->setModel(model);
}

QLineEdit *UISearch::searchBar()
{
    return ui->searchBar;
}

bool UISearch::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == ui->searchBar){ // 如果事件源是搜索栏
        if(event->type() == QEvent::FocusIn){ // 当搜索栏获得焦点时，显示搜索预览面板
            showPreviewPanel();
        }
        else if(event->type() == QEvent::FocusOut){
            if(m_previewPanel && m_previewPanel->isVisible() && m_previewPanel->underMouse()){
                return true;
            }

            hidePreviewPanel();
        }
    }

    if (event->type() == QEvent::MouseButtonPress && m_previewPanel && m_previewPanel->isVisible()) {
        if (!ui->searchBar->underMouse() && !m_previewPanel->underMouse()) {
            hidePreviewPanel();
        }
    }


    return QWidget::eventFilter(watched, event);
}
