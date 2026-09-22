#ifndef PAGE_H
#define PAGE_H

/**
 * @brief 主页面类型
 */
enum MainPage {
    Local = 0, ///< 本地数据页
    Collect, ///< 收藏页
    PlayList, ///< 歌单页
    Remote, ///< 远程数据页
};

/**
 * @brief 搜索页面类型
 */
enum SearchPage {
    Main = 0, ///< 搜索主页
    Search ///< 搜索结果页
};

/**
 * @brief 代理模型 id
 */
enum class ProxyId : int {
    Local = 0, ///< LocalProxyModel
    Remote = 1, ///< RemoteProxyModel
    Collect = 2, ///< CollectProxyModel
    Playlist = 3, ///< PlayListProxyModel
    Search = 4 ///<  SearchProxyModel
};


#endif // PAGE_H
