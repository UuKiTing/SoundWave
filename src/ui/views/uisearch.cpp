#include "uisearch.h"
#include "ui_uisearch.h"
#include "global.h"
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

    window()->installEventFilter(this); // 安装事件过滤器到顶层窗口，以便在点击其他地方时隐藏预览面板
}

UISearch::~UISearch()
{
    delete ui;
}

void UISearch::connectSignal()
{
    // 搜索列表双击播放事件
    connect(m_previewPanel->searchListView(), &QListView::doubleClicked, this, &UISearch::doubleClickPlay);

    // 搜索栏文本变化事件
    connect(ui->searchBar, &QLineEdit::textChanged, this, &UISearch::switchStackedWidget);
}

void UISearch::doubleClickPlay(const QModelIndex &index)
{
    // 延迟隐藏预览面板，避免在双击时立即隐藏导致的界面闪烁
    QTimer::singleShot(100, this, &UISearch::hidePreviewPanel);

    // 获取搜索列表的模型，并将索引映射到源模型
    QSortFilterProxyModel *model = qobject_cast<QSortFilterProxyModel*>(m_previewPanel->searchListView()->model());

    // 发射播放歌曲请求信号，传递源模型索引和自动播放标志
    emit songPlayRequest(model->mapToSource(index), true);
}

void UISearch::switchStackedWidget(QString text)
{
    if(text.isEmpty()) m_previewPanel->switchStackedWidget(SearchPage::Main); // 如果搜索栏为空，则切换到搜索列表主界面
    else m_previewPanel->switchStackedWidget(SearchPage::Search); // 如果搜索栏不为空，则切换到搜索列表的结果页面
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
    QPoint globalPos = ui->searchBar->mapToGlobal(QPoint(0, ui->searchBar->height())); // 计算搜索列表的位置
    m_previewPanel->move(globalPos); // 将搜索列表移动到搜索栏下方
    m_previewPanel->resize(ui->searchBar->width(), 200); // 设置搜索列表的宽度和高度
    m_previewPanel->show(); // 显示搜索列表
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


void UISearch::on_searchBtn_clicked()
{
}


void UISearch::on_searchBar_returnPressed()
{
}


bool UISearch::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == ui->searchBar){ // 如果事件源是搜索栏
        if(event->type() == QEvent::FocusIn){ // 当搜索栏获得焦点时，显示预览面板
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
