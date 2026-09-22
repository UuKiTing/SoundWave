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

    /** @brief 显示搜索预览面板 */
    void showPreviewPanel();

    /** @brief 隐藏搜索预览面板 */
    void hidePreviewPanel();

    /** @brief 设置模型 */
    void setModel(QAbstractItemView *view, QAbstractItemModel *model);

    /** @brief 获取搜索栏的文本 */
    QString searchText();

    /** @brief 获取搜索视图 */
    QAbstractItemView* searchListView();

    /** @brief 获取搜索栏 */
    QLineEdit* searchBar();

signals:
    /** @brief 歌曲播放信号 */
    void songPlayRequest(const QModelIndex &index, bool autoPlay);

    void flushed();

protected:
    /** @brief 事件过滤器 */
    bool eventFilter(QObject *watched, QEvent *event) override;


private slots:
    /** @brief 点击搜索事件 */
    void on_searchBtn_clicked();

    /** @brief 回车搜索事件 */
    void on_searchBar_returnPressed();

private:
    /** @brief 连接信号和槽 */
    void connectSignal();

    /** @brief 双击播放 */
    void doubleClickPlay(const QModelIndex &index);

    /** @brief 搜索预览面板页面切换 */
    void switchStackedWidget(QString text);

    Ui::UISearch *ui;

    SearchPreviewPanel *m_previewPanel{}; ///< 搜索预览面板

    SearchBarDelegate *m_delegate{}; ///< 自定义搜索代理
};

#endif // UISEARCH_H
