# QtVideoPlayer

> 个人学习项目，用于学习 **Qt GUI 开发、libVLC 媒体播放集成、OpenSSL 加密、HTTP 网络请求、多线程、自定义窗口控件** 等技术栈。

---

## 项目概述

QtVideoPlayer 是一个基于 **Qt5 + libVLC** 实现的桌面视频播放器客户端，具备登录鉴权、本地/网络媒体播放、加密媒体文件解密播放、播放列表管理、全屏控制等完整功能。项目采用 C++11 开发，目标平台为 Windows。

---

## 整体架构

```
QtVideoPlayer
├── 登录层        LoginForm          登录窗口，HTTP 请求鉴权，记住密码/自动登录
├── 主窗口层      Widget             核心播放界面，事件处理，窗口尺寸/全屏管理
├── 播放器封装    QMediaPlayer       对 vlchelper 的 Qt 信号槽封装，对外暴露统一接口
├── VLC 接入层    vlchelper          封装 libVLC C API，控制播放、音量、倍速、截帧
│               vlcmedia           自定义 VLC 媒体 IO 回调，支持加密文件透明解密
├── 媒体抽象层    MediaBase          媒体读取抽象基类（open/read/seek/close）
│               MediaMP4           本地 MP4 加密文件实现，按位置解密数据流
├── 安全模块      SslTool            OpenSSL RSA 公/私钥加解密
│               BigNum             BIGNUM 封装，支持 Base64/Hex/Binary 互转
│               RecordFile         本地配置文件读写，JSON + 加密存储
├── 网络模块      QNetworkAccessManager  Qt 原生 HTTP 请求（登录、心跳保活）
├── 辅助控件      QScreenTop         全屏顶部悬浮提示条
│               QStatusCheck       独立线程周期性状态检查（登录状态保活）
│               InfoForm           信息提示浮层
│               MessageForm        全屏模式下的顶部半透明信息栏
└── 资源          UI/images          所有图标和皮肤图片资源（qrc 打包）
```

---

## 技术要点

### 1. Qt GUI 与自定义窗口
- 无边框窗口（Frameless Window），自实现鼠标拖拽移动、边缘拖拽缩放（8方向）
- 内嵌 `SizeHelper` 类统一管理普通/最大化/全屏三种状态下各控件的尺寸与显隐逻辑
- `eventFilter` 拦截子控件事件，实现进度条悬停预览等交互
- Qt 国际化支持（`.ts` / `.qm` 翻译文件）

### 2. libVLC 集成
- 直接调用 libVLC C API（`libvlc_instance_t`、`libvlc_media_player_t`）
- 通过 `libvlc_media_new_callbacks` 注册自定义 IO 回调（open/read/seek/close），实现对加密文件的透明解密播放，VLC 无需感知文件格式
- 支持悬浮 Logo 水印与悬浮文字字幕叠加
- 支持播放倍速控制、时间跳转、音量/静音切换、截取指定时间帧图片

### 3. 媒体抽象与加密解密
- `MediaBase` 定义纯虚接口，解耦 VLC IO 层与具体媒体实现
- `MediaMP4` 继承 `MediaBase`，在 `read()` 时对数据流按偏移量进行异或/流式解密（`decrypto`），实现加密 MP4 文件的边读边解密

### 4. OpenSSL 加密模块
- 封装 OpenSSL RSA 公钥加密 / 私钥解密接口（`SslTool::rsaEncode` / `rsaDecode`）
- `BigNum` 类封装 OpenSSL `BIGNUM`，支持 Base64、Hex、Binary 格式互转
- 本地配置文件（`RecordFile`）使用加密方式持久化用户账号密码等敏感信息

### 5. 网络与鉴权
- 使用 `QNetworkAccessManager` 发起 HTTP POST 登录请求，解析 JSON 响应
- 登录成功后通过 Qt 信号（`login(nick, head)`）通知主窗口显示用户昵称与头像
- `QStatusCheck`（继承 `QThread`）独立线程周期性发送心跳请求，维持登录态
- `keep_activity()` 定时器防止系统休眠或网络断线

### 6. 信号槽驱动的播放器封装
- `QMediaPlayer` 继承 `QLabel`（作为视频渲染容器），将所有播放控制封装为槽函数
- 对外通过信号发布播放进度（`position(double)`）和状态变更（`media_status(PlayerStatus)`）
- `Widget` 通过 Qt 信号槽与 `QMediaPlayer` 完全解耦，控制指令与状态回调分离

### 7. 播放列表与交互
- 支持文件拖拽（`dragEnterEvent` / `dropEvent`）批量添加媒体到播放列表
- 双击列表项立即切换播放，支持上一首/下一首切换
- 倍速按钮（0.5x / 1x / 1.5x / 2x）与键盘快捷键控制

---

## 分支说明

### `master`
主分支，包含项目完整代码。集成了上述所有模块：登录鉴权、加密媒体播放、自定义无边框窗口、OpenSSL 加密存储、网络心跳保活等功能的完整实现。
