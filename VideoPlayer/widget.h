#ifndef WIDGET_H
#define WIDGET_H
#include <windows.h>
#include <QWidget>
#include <QSlider>
#include <QDropEvent>
#include <QList>
#include <QListWidgetItem>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QRandomGenerator>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonDocument>
#include "vlchelper.h"
#include "ssltool.h"
#include "qmediaplayer.h"
#include "infoform.h"
#include "messageform.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
    class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget* parent = nullptr);
    ~Widget();
    // 定时器事件处理，用于周期性地更新界面或执行后台任务
    virtual void timerEvent(QTimerEvent* event);
protected:
    void save_default_rect_info();
    // 拖拽进入事件处理，允许用户拖拽文件到窗口
    void dragEnterEvent(QDragEnterEvent* event);
    // 拖拽释放事件处理，解析拖入的文件（如媒体文件）并加入播放列表
    void dropEvent(QDropEvent* event);
    void handleTimeout(int nTimerID);
    // 鼠标事件处理
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    // 事件过滤器，拦截特定控件的事件
    virtual bool eventFilter(QObject* watched, QEvent* event);
    // 键盘按键事件处理
    virtual void keyPressEvent(QKeyEvent* event);
private:
    // 初始化滑动条的范围和步长（如进度条、音量条）
    void setSlider(QSlider* slider, int nMin, int nMax, int nStep);
    //设置当前播放的时间
    void setTime(int hour, int minute, int seconds);
    void setTime(int64_t tm);
    //设置媒体总的时长
    void setTime2(int hour, int minute, int seconds);
    void setTime2(int64_t tm);
    // 绘制界面分隔线或装饰性元素
    void paintLine();
    // 初始化媒体播放器模块
    void init_media();
signals: // 信号
    // 通知播放进度更新
    void update_pos(double pos); 

    // 控制媒体播放状态
    void open(const QUrl& url);
    void play();
    void pause();//暂停
    void stop();//停止播放
    void close_media();//关闭媒体

    // 调整播放进度、速度和音量
    void seek(double pos);
    void set_position(int64_t pos);//设置播放位置 单位毫秒
    void set_position(//设置播放位置，时分秒毫秒
        int hour, int minute, int second, int millisecond
    );
    void set_scale(float scale);//设置播放倍率
    //取指定位置(单位毫秒)的一帧图片（用于预览）
    void pick_frame(QImage& frame, int64_t pos);
    //改变播放窗口大小
    void set_size(const QSize& sz);
    void volume(int vol);
    void silence();
public slots: // 槽函数
    // 响应登录成功信号，显示主窗口并更新用户信息（昵称和头像）
    void on_show(const QString& nick, const QByteArray& head);
    void slot_connect_clicked();//联系售后
protected slots:
    void on_nextButton_clicked(); // 切换下一个媒体
    void on_preButton_clicked(); // 切换上一个媒体
    void on_playButton_clicked(); //播放/暂停
    void on_volumeButton_clicked(); // 静音/取消静音（可能通过按钮图标切换表示状态）
    void on_scaleButton_clicked();
    void on_horizontalSlider_sliderPressed(); //进度条被按下时暂停自动更新
    void on_horizontalSlider_sliderReleased(); //进度条释放后跳转到指定位置
    void on_horizontalSlider_rangeChanged(int min, int max);
    void on_horizontalSlider_valueChanged(int value); // 实时更新进度条显示
    void on_media_position(double pos);
    void on_media_media_status(QMediaPlayer::PlayerStatus s);
    void slots_login_request_finshed(QNetworkReply* reply);
private slots:
    void on_listWidget_itemDoubleClicked(QListWidgetItem* item);//双击播放列表项时立即播放该文件
    void on_volumeSlider_sliderReleased(); // 音量滑块释放时生效
    void on_volumeSlider_valueChanged(int value); // 实时响应音量滑块值变化（如拖动时即时调整音量）
    void on_showhideList_pressed(); // 显示/隐藏播放列表（可能通过按钮切换状态）
    void on_closeButton_released(); // 关闭窗口或退出程序
    void on_fangdaButton_clicked(); // 最大化窗口（非全屏）
    void on_fullscreenBtn_clicked(); // 进入全屏模式
    void on_stopButton_clicked(); // 停止播放并重置进度
    // 设置播放速度（0.5倍、1倍、1.5倍、2倍）
    void on_time2_clicked();
    void on_time1_5_clicked();
    void on_time1_clicked();
    void on_time0_5_clicked();
    // 最小化窗口
    void on_miniButton_clicked();

private:
    // 逐步加速或减速播放
    void on_fastButton_clicked();
    void on_slowButton_clicked();
    // 维持程序活跃状态（防止系统休眠或断网）
    bool keep_activity();
private:
    //尺寸助手 管理窗口大小调整、全屏/最大化逻辑和控件布局
    class SizeHelper
    {
    public:
        SizeHelper(Widget* ui);
        ~SizeHelper();
        //当调整完大小的时候更新
        void update(int nWidth, int nHeight);
        //获取当前的鼠标光标类型 0 正常 1 左上 2 顶 3 右上 4 右 5 右下 6 底 7 左下 8 左
        void modify_mouse_cousor(const QPoint& point);
        //设置false禁用本功能（全屏时禁用）true开启功能，默认为true
        void set_enable(bool isable = true);
        int type()
        {
            return index;
        }
        void press(const QPoint& point);
        void move(const QPoint& point);
        void release();
        void init_size_info(QWidget* widget);
        void set_full_rect(const QString& name, const QRect& rect);
        void set_max_rect(const QString& name, const QRect& rect);
        void set_org_rect(const QString& name, const QRect& rect);
        void set_last_rect(const QString& name, const QRect& rect);
        void set_full_hide(const QString& name, bool is_hide = true);
        void set_max_hide(const QString& name, bool is_hide = true);
        void set_auto_hide(const QString& name, int hide_status = 1);
        void full_size();
        void org_size();
        void max_size();
        int  cur_status() const;
        void auto_hide(bool hidden = true);
    protected:
        struct SizeInfo
        {
            bool is_full_hide;//全屏隐藏
            bool is_max_hide;//最大化隐藏
            int auto_hide_status;//全屏时自动隐藏的控件 0 不隐藏 1 隐藏 2 不参与（该控件在全屏模式下不存在)
            QRect org_rect;//原始尺寸
            QRect max_rect;//最大化尺寸
            QRect full_rect;//全屏尺寸
            QRect last_rect;//上一个尺寸
            QWidget* widget;//控件
            SizeInfo()
            {
                widget = NULL;
                is_full_hide = false;
                is_max_hide = false;
            }
        };
    protected:
        //窗口调整属性
        QRect size_rect[8];//8个区域，会触发鼠标变化，用于改变ui整体窗口大小
        bool isabled;//默认为true
        int curent_coursor;//当前光标类型，默认为0，表示普通光标
        QCursor* cursors[9];//光标
        int index;//当前起效的光标
        QPoint point;//按下的起点
        bool pressed;//按下状态
        Widget* ui;
        QMap<QString, SizeInfo> sub_item_size;
        int status;//当前屏幕状态0 普通 1 最大化 2 全屏
    };
private:
    Ui::Widget* ui;
    QList<QUrl> mediaList;
    //vlchelper* vlc;
    SslTool ssl_tool;
    int current_play;
    int timerID;
    int timesCount;
    int timesID;//倍速按钮定时器
    int volumeSliderTimerID;
    int fullScreenTimerID;
    int keep_activity_timerID;
    int volumeCount;
    QPoint position;
    float rate;
    bool slider_pressed;
    qint16 tick;
    SizeHelper helper;
    InfoForm info;
    MessageForm* full_head;//全屏时的头部半透明框
    int screentopTimerID;
    QNetworkAccessManager* net;
    QString nick;//昵称
};
#endif // WIDGET_H
