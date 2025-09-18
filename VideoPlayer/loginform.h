#ifndef LOGINFORM_H
#define LOGINFORM_H

#include <QWidget>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QDesktopServices>

#include "infoform.h"
#include "recordfile.h"


namespace Ui
{
    class LoginForm;
}

class LoginForm : public QWidget
{
    Q_OBJECT
    friend class testVideoPlayer;
public:
    explicit LoginForm(QWidget* parent = nullptr);
    ~LoginForm();
    virtual bool eventFilter(QObject* watched, QEvent* event);// 事件过滤器
    virtual void timerEvent(QTimerEvent* event);// 定时器事件
protected slots:
    void on_logoButton_released();// 点击 logo 按钮
    void on_remberPwd_stateChanged(int state);// “记住密码”复选框状态变化
    void slots_autoLoginCheck_stateChange(int state);// “自动登录”复选框状态变化
    void slots_login_request_finshed(QNetworkReply* reply);// 登录请求完成回调
signals:
    void login(const QString& nick, const QByteArray& head);// 当用户登录成功时，发射信号传递昵称 (nick) 和头像数据 (head)，供其他窗口（如主界面）接收
protected:
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
private:
    // 检查用户名/密码是否为空或格式正确
    bool check_login(const QString& user, const QString& pwd);
    // 从文件或注册表读取“记住密码”和“自动登录”设置
    void load_config();
private:
    Ui::LoginForm* ui;
    QNetworkAccessManager* net;
    InfoForm info;
    RecordFile* record;
    int auto_login_id;
    QPoint position;
};

#endif // LOGINFORM_H
