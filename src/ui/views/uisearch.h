#ifndef UISEARCH_H
#define UISEARCH_H

#include "searchbar_delegate.h"
#include "search_preview_panel.h"
#include <QWidget>
#include <QListView>

namespace Ui {
class UISearch;
}

class UISearch : public QWidget
{
    Q_OBJECT

public:
    explicit UISearch(QWidget *parent = nullptr);
    ~UISearch();

    void showPreviewPanel(); // 显示搜索预览面板
    void hidePreviewPanel(); // 隐藏搜索预览面板
    void setModel(QAbstractItemView *view, QAbstractItemModel *model); // 设置模型

    QString searchText(); // 获取搜索栏的文本
    QAbstractItemView* searchListView(); // 获取搜索列表视图
    QLineEdit* searchBar(); // 获取搜索栏

signals:
    void songPlayRequest(const QModelIndex &index, bool autoPlay); // 播放歌曲请求信号

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;


private slots:
    void on_searchBtn_clicked(); // 搜索按钮点击槽函数

    void on_searchBar_returnPressed(); // 搜索栏回车按下槽函数

private:
    void connectSignal(); // 连接信号和槽
    void doubleClickPlay(const QModelIndex &index); // 双击播放功能
    void switchStackedWidget(QString text); // 切换堆叠窗口

    Ui::UISearch *ui;

    SearchPreviewPanel *m_previewPanel{};

    SearchBarDelegate *m_delegate{}; // 自定义搜索代理
};

#endif // UISEARCH_H
