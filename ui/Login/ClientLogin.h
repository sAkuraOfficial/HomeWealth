#pragma once
#include "AnimatedStackedWidget.hpp"
#include "ui_ClientLogin.h"
#include <QtWidgets/QMainWindow>
#include <core/core.h>
#include <qevent.h>
#include <qmovie.h>
class ClientLogin : public QMainWindow
{
    Q_OBJECT
  protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

  public:
    ClientLogin(Core *core, QWidget *parent = nullptr);
    ~ClientLogin();

  private:
    Ui::ClientLoginClass ui;
    Core *m_core = nullptr;
    QMovie *movie;
    QMovie *movie_b;
    bool isLoginPage = true;
    bool isBackendPage = false;
    QString m_backend_ip;
    QString m_backend_port;
    QTimer *animationTimer = nullptr; // 用于加载动画的定时器

  private slots:
    void on_pushButton_login_clicked();      // 登录按钮
    void on_pushButton_reg_clicked();        // 注册按钮
    void on_pushButton_login_test_clicked(); // 测试登录按钮
    void onBeginConnect();                   // 开始连接,显示加载动画
    void onConnectTimeOut();                 // 连接超时
    void onConnectSuccess();                 // 连接成功
    void onDisconnected();
    void onReceiveLoginResult(bool result, QString username, int user_id, bool is_admin, int family_id); // 处理登录结果
    void onReceiveRegisterResult(bool result, int user_id);                                              // 处理注册结果
    void on_pushButton_set_backend_ip_clicked();                                                         // 设置后端IP按钮
    void on_checkBox_autoLogin_clicked();                                                                // 自动登录复选框
    void on_checkBox_rememberPwd_clicked();

  signals:
    void LoginSuccess(QString username, int user_id, bool is_admin, int family_id); // 登陆成功，显示聊天窗口，本信号提供给主窗口使用
};
