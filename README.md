# SoundWave

基于 Qt 6 的桌面音乐播放器，支持本地播放与云端曲库，数据存储在 SQLite 中。

## 功能

- 本地音乐播放：播放/暂停、上下首、进度拖拽、音量调节
- 播放模式：列表循环、单曲循环、随机
- 歌单：新建/删除/重命名、添加与移除歌曲
- 收藏：一键收藏，独立的收藏列表
- 云端曲库：在线歌单浏览、搜索（标题/歌手）、下载音频与封面歌词
- 歌词页：LRC 解析，随播放进度滚动高亮
- 定时关闭：预设时长或自定义
- 系统托盘：双击托盘恢复、右键退出

## 运行

下载发行版压缩包，解压后双击 `SoundWave.exe` 即可，无需安装 Qt 或配置环境。

首次启动会在程序目录下自动创建 `data/` 数据目录。

## 配置

程序目录下放 `url.config` 指定服务端地址，缺失时使用默认值：

```ini
BaseUrl=http://192.168.85.128:8080
```

## 数据

运行目录下的 `data/` 由程序自动创建：

```
data/
├── music.db        SQLite 数据库
├── songAudio/      下载的音频
├── songImage/      封面
├── songLyrics/     歌词
└── logs/           按天滚动的日志
```

用户偏好（音量、播放模式、进度等）保存在 `QSettings`（Windows 注册表 `HKCU\Software\Luo\SoundWave`）。

## 目录结构

```
src/
├── app/        应用装配、设置持久化、信号槽中介者
├── service/    播放器控制、曲库管理、SQLite 仓储、HTTP 与下载
├── types/      数据结构与枚举
├── ui/         主窗口、播放栏、侧边栏、歌词页、自定义控件
└── utils/      路径、日志、封面缓存、样式表等工具
```

源码基于 CMake ≥ 3.16 与 Qt 6（`Core`、`Gui`、`Widgets`、`Multimedia`、`Sql`、`Concurrent`、`Network`），仅在你希望自行编译时才需要。

## 注意
- 云端功能依赖 `BaseUrl` 指向的服务端可用；本地播放不依赖服务端。
- 如需使用云端功能可自行下载[AudioStreamServer项目](https://github.com/UuKiTing/AudioStreamServer)并运行。

