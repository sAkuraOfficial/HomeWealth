#include "ClientLogin.h"
#include <QFile>
#include <QTimer>
#include <qwebsocket.h>
#include <ui/HomeWealth/HomeWealth.h>
ClientLogin::ClientLogin(Core *core, QWidget *parent)
    : QMainWindow(parent), m_core(core)
{
    ui.setupUi(this);
    ui.label_welcome_login->setText("家庭理财管理系统");
    ui.label_welcome_reg->setText("注册");
    animationTimer = new QTimer(this);

    // 连接服务器
    connect(m_core, &Core::beginConnect, this, &ClientLogin::onBeginConnect);                                      // 连接开始时显示加载动画
    connect(m_core, &Core::ConnectTimeOut, this, &ClientLogin::onConnectTimeOut);                                  // 连接超时处理
    connect(m_core, &Core::ConnectSuccess, this, &ClientLogin::onConnectSuccess);                                  // 连接成功处理
    connect(m_core->getProtocol()->getWebSocket(), &QWebSocket::disconnected, this, &ClientLogin::onDisconnected); // 断开连接处理

    // 登录
    connect(m_core, &Core::ReceiveLoginResult, this, &ClientLogin::onReceiveLoginResult); // 处理登录结果

    // m_core->runClient("ws://114.132.98.222:2222", 5000); // 连接到服务器，超时时间为5秒
     m_core->runClient("ws://127.0.0.1:2222", 5000); // 连接到服务器，超时时间为5秒
    // 判断qrc资源是否有效:/Client/login_a
    // if (!QFile::exists(":/Client/login_a"))
    //    qDebug() << "qrc资源无效";
    // else
    //    qDebug() << "qrc资源有效";

    movie = new QMovie(":/Client/login_a");
    movie_b = new QMovie(":/Client/login_b");
    ui.stackedWidget->setCurrentWidget(ui.page_login);

    // gif_a与gif_b不显示
    ui.gif_a->hide();
    ui.gif_b->hide();

    ui.lineEdit_pwd->installEventFilter(this);
    ui.label_link_reg->installEventFilter(this);
    ui.label_link_status->installEventFilter(this);
}

ClientLogin::~ClientLogin()
{
}

bool ClientLogin::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == ui.lineEdit_pwd)
    {
        if (event->type() == QEvent::FocusIn)
        {
            // 当 lineEdit_pwd 获得焦点时启动动画
            ui.gif_a->setMovie(movie);
            ui.gif_b->setMovie(movie_b);
            movie->setScaledSize(ui.gif_a->size());
            movie_b->setScaledSize(ui.gif_b->size());
            movie->start();
            movie_b->start();
            ui.gif_a->show();
            ui.gif_b->show();
        }
        else if (event->type() == QEvent::FocusOut)
        {
            // 当 lineEdit_pwd 失去焦点时停止动画
            // 不显示label
            ui.gif_a->clear();
            ui.gif_b->clear();
            movie->stop();
            movie_b->stop();
            ui.gif_a->hide();
            ui.gif_b->hide();
        }
    }
    if (watched == ui.label_link_reg)
    {
        if (event->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
            if (mouseEvent->button() == Qt::LeftButton)
            {
                // 当 label_link_reg 被点击时切换页面
                if (isLoginPage)
                {
                    ui.stackedWidget->setCurrentWidget(ui.page_signin);
                }
                else
                {
                    ui.stackedWidget->setCurrentWidget(ui.page_login);
                }
                isLoginPage = !isLoginPage; // 切换标记
            }
        }
    }
    if (watched == ui.label_link_status)
    {
        if (event->type() == QEvent::MouseButtonPress)
        {
            // 当 label_link_status 被点击时重新连接
            if (m_core->isConnecting() == false)
                m_core->runClient("ws://localhost:2222", 5000); // 连接到服务器，超时时间为5秒
        }
    }
    return QMainWindow::eventFilter(watched, event);
}

void ClientLogin::on_pushButton_login_clicked()
{
    m_core->login(ui.lineEdit_id->text(), ui.lineEdit_pwd->text());
}

void ClientLogin::on_pushButton_reg_clicked()
{
    m_core->registerUser(ui.lineEdit_reg_id->text(), ui.lineEdit_reg_pwd->text());
}

void ClientLogin::on_pushButton_login_test_clicked()
{
    m_core->login("a", "a");
}

void ClientLogin::onBeginConnect()
{
    ui.label_link_status->setText("服务器状态：连接中"); // 更新 QLabel 的文本

    animationTimer->setInterval(50); // 每500毫秒更新一次
    connect(animationTimer, &QTimer::timeout, this, [this]() {
        static int step = 0;
        QStringList states = {"连接中", "连接中.", "连接中..", "连接中...", "连接中...."};
        ui.label_link_status->setText("服务器状态：" + states[step]);
        step = (step + 1) % states.size();
    });

    animationTimer->start(); // 启动动画
}

void ClientLogin::onConnectTimeOut()
{
    animationTimer->stop(); // 停止动画
    ui.label_link_status->setText("服务器状态：连接超时");
}

void ClientLogin::onConnectSuccess()
{
    animationTimer->stop(); // 停止动画
    ui.label_link_status->setText("服务器状态：连接成功");
}

void ClientLogin::onDisconnected()
{
    ui.label_link_status->setText("服务器状态：断开连接");
}

void ClientLogin::onReceiveLoginResult(bool result, QString username, int user_id, bool is_admin, int family_id)
{
    if (result)
    {
        QString username = ui.lineEdit_id->text();
        emit LoginSuccess(username, user_id,is_admin,family_id); // 传递登陆成功信号
        this->close();                        // 隐藏当前窗口
    }
    else
    {
        // 登录失败
        ui.label_link_status->setText("服务器状态：登录失败");
        ui.lineEdit_pwd->clear();
    }
}