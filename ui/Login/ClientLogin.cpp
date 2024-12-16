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
    /*   ui.stackedWidget->setCurrentIndex(0);
       ui.stackedWidget->slideToIndex(0);*/
    isLoginPage = true;
    isBackendPage = false;

    animationTimer = new QTimer(this);

    // 连接服务器
    connect(m_core, &Core::beginConnect, this, &ClientLogin::onBeginConnect);                                      // 连接开始时显示加载动画
    connect(m_core, &Core::ConnectTimeOut, this, &ClientLogin::onConnectTimeOut);                                  // 连接超时处理
    connect(m_core, &Core::ConnectSuccess, this, &ClientLogin::onConnectSuccess);                                  // 连接成功处理
    connect(m_core->getProtocol()->getWebSocket(), &QWebSocket::disconnected, this, &ClientLogin::onDisconnected); // 断开连接处理

    // 登录
    connect(m_core, &Core::ReceiveLoginResult, this, &ClientLogin::onReceiveLoginResult); // 处理登录结果
    connect(m_core, &Core::ReceiveRegisterResult, this, &ClientLogin::onReceiveRegisterResult);

    // m_core->runClient("ws://114.132.98.222:2222", 5000); // 连接到服务器，超时时间为5秒
    // m_core->runClient("ws://127.0.0.1:2222", 5000); // 连接到服务器，超时时间为5秒

    // 从json文件中读取ip和port，若文件不存在则使用默认值
    QFile file("backend_ip.json");
    if (file.open(QIODevice::ReadOnly))
    {
        QByteArray data = file.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        QJsonObject json = doc.object();
        m_backend_ip = json.value("ip").toString();
        m_backend_port = json.value("port").toString();
        file.close();
    }
    else
    {
        m_backend_ip = "114.132.98.222";
        m_backend_port = "2222";
        // 构建为json，保存到当前程序运行目录下
        QJsonObject json;
        json.insert("ip", m_backend_ip);
        json.insert("port", m_backend_port);
        QJsonDocument doc(json);
        if (file.open(QIODevice::WriteOnly))
        {
            file.write(doc.toJson());
            file.close();
        }
        else
        {
            qDebug() << "打开文件失败";
        }
    }
    ui.lineEdit_ip->setText(m_backend_ip);
    ui.lineEdit_port->setText(m_backend_port);
    m_core->runClient("ws://" + m_backend_ip + ":" + m_backend_port, 5000); // 连接到服务器，超时时间为5秒

    // 从login.json中读取数据
    QFile file_login("login.json");
    if (file_login.open(QIODevice::ReadOnly))
    {
        QByteArray data = file_login.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        QJsonObject json = doc.object();
        ui.checkBox_autoLogin->setChecked(json.value("autoLogin").toBool());
        ui.checkBox_rememberPwd->setChecked(json.value("rememberPwd").toBool());
        ui.lineEdit_id->setText(json.value("user_name").toString());
        ui.lineEdit_pwd->setText(json.value("password").toString());
        file_login.close();
    }
    // 若不存在则不管

    movie = new QMovie(":/Client/login_a");
    movie_b = new QMovie(":/Client/login_b");
    ui.stackedWidget->setCurrentWidget(ui.page_login);

    // gif_a与gif_b不显示
    ui.gif_a->hide();
    ui.gif_b->hide();

    ui.lineEdit_pwd->installEventFilter(this);
    ui.label_link_reg->installEventFilter(this);
    ui.label_link_status->installEventFilter(this);
    ui.label_link_backend->installEventFilter(this);
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
                if (isBackendPage)
                {
                    if (isLoginPage)
                    {
                        ui.stackedWidget->slideToIndex(0); // 登录
                    }
                    else
                    {
                        ui.stackedWidget->slideToIndex(1); // 注册
                    }
                    isBackendPage = false;
                }
                else
                {
                    // 当 label_link_reg 被点击时切换页面
                    if (isLoginPage)
                    {
                        // ui.stackedWidget->setCurrentWidget(ui.page_signin);
                        ui.stackedWidget->slideToIndex(1); // 使用动画切换页面
                    }
                    else
                    {
                        // ui.stackedWidget->setCurrentWidget(ui.page_login);
                        ui.stackedWidget->slideToIndex(0); // 使用动画切换页面
                    }
                    isLoginPage = !isLoginPage; // 切换标记
                }
            }
        }
    }
    if (watched == ui.label_link_status)
    {
        if (event->type() == QEvent::MouseButtonPress)
        {
            // 当 label_link_status 被点击时重新连接
            if (m_core->isConnecting() == false)
            {
                m_core->runClient("ws://" + m_backend_ip + ":" + m_backend_port, 5000); // 连接到服务器，超时时间为5秒
            }
        }
    }
    if (watched == ui.label_link_backend)
    {
        if (event->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
            if (mouseEvent->button() == Qt::LeftButton)
            {
                if (isBackendPage)
                {
                    // 代表返回之前的页面
                    if (isLoginPage)
                        ui.stackedWidget->slideToIndex(0); // 登录
                    else
                        ui.stackedWidget->slideToIndex(1); // 注册
                    isBackendPage = false;
                }
                else
                {
                    ui.stackedWidget->slideToIndex(2); // 后台
                    isBackendPage = true;
                }
            }
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
    bool is_admin;
    if (ui.radioButton_admin->isChecked())
    {
        is_admin = true;
    }
    else
    {
        is_admin = false;
    }
    m_core->registerUser(ui.lineEdit_reg_id->text(), ui.lineEdit_reg_pwd->text(), is_admin);
}

void ClientLogin::on_pushButton_login_test_clicked()
{
    m_core->login("周子豪", "123456");
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
    if (isBackendPage)
    {
        if (isLoginPage)
        {
            ui.stackedWidget->slideToIndex(0); // 登录
        }
        else
        {
            ui.stackedWidget->slideToIndex(1); // 注册
        }
    }
}

void ClientLogin::onDisconnected()
{
    ui.label_link_status->setText("服务器状态：断开连接");
}

void ClientLogin::onReceiveLoginResult(bool result, QString username, int user_id, bool is_admin, int family_id)
{
    if (result)
    {
        emit LoginSuccess(username, user_id, is_admin, family_id); // 传递登陆成功信号
        //写出login.json
        QJsonObject json;
        json["autoLogin"] = ui.checkBox_autoLogin->isChecked();
        json["rememberPwd"] = ui.checkBox_rememberPwd->isChecked();
        json["user_name"] = ui.lineEdit_id->text();
        json["password"] = ui.lineEdit_pwd->text();
        QJsonDocument doc;
        doc.setObject(json);
        QFile file("login.json");
        if (file.open(QIODevice::WriteOnly))
        {
            file.write(doc.toJson());
            file.close();
        }
        else
        {
            qDebug() << "打开文件失败";
        }

        this->close();                                             // 隐藏当前窗口
    }
    else
    {
        // 登录失败
        ui.label_link_status->setText("服务器状态：登录失败");
        ui.lineEdit_pwd->clear();
    }
}

void ClientLogin::onReceiveRegisterResult(bool result, int user_id)
{
    if (result)
    {
        ui.label_link_status->setText("注册成功");
        ui.stackedWidget->slideToIndex(0); // 注册成功后切换到登录页面
        ui.lineEdit_id->setText(ui.lineEdit_reg_id->text());
        ui.lineEdit_pwd->setText(ui.lineEdit_reg_pwd->text());
    }
    else
    {
        ui.label_link_status->setText("注册失败");
    }
}

void ClientLogin::on_pushButton_set_backend_ip_clicked()
{
    QString ip = ui.lineEdit_ip->text();
    QString port = ui.lineEdit_port->text();
    m_backend_ip = ip;
    m_backend_port = port;
    // 构建为json，保存到当前程序运行目录下
    QJsonObject json;
    json.insert("ip", ip);
    json.insert("port", port);
    QJsonDocument doc(json);
    QFile file("backend_ip.json");
    if (file.open(QIODevice::WriteOnly))
    {
        file.write(doc.toJson());
        file.close();
    }
    else
    {
        qDebug() << "打开文件失败";
    }
    m_core->getProtocol()->disconnect_to_server();
    m_core->runClient("ws://" + ip + ":" + port, 5000); // 连接到服务器，超时时间为5秒
}

void ClientLogin::on_checkBox_autoLogin_clicked()
{
    if (ui.checkBox_autoLogin->isChecked())
    {
        ui.checkBox_rememberPwd->setChecked(true);
    }
    else
    {
        // 不需要改变记住密码状态
    }
    // 在运行目录下写出:login.json
    QJsonObject json;
    json["autoLogin"] = ui.checkBox_autoLogin->isChecked();
    json["rememberPwd"] = ui.checkBox_rememberPwd->isChecked();
    json["user_name"] = ui.lineEdit_id->text();
    json["password"] = ui.lineEdit_pwd->text();
    QJsonDocument doc;
    doc.setObject(json);
    QFile file("login.json");
    if (file.open(QIODevice::WriteOnly))
    {
        file.write(doc.toJson());
        file.close();
    }
    else
    {
        qDebug() << "打开文件失败";
    }
}

void ClientLogin::on_checkBox_rememberPwd_clicked()
{
    // 等待写出的数据
    QString user_name = "";
    QString password = "";
    if (ui.checkBox_rememberPwd->isChecked())
    {
        // 不需要改变自动登录状态
        user_name = ui.lineEdit_id->text();
        password = ui.lineEdit_pwd->text();
    }
    else
    {
        ui.checkBox_autoLogin->setChecked(false);
        user_name = "";
        password = "";
    }
    // 在运行目录下写出:login.json
    QJsonObject json;
    json["autoLogin"] = ui.checkBox_autoLogin->isChecked();
    json["rememberPwd"] = ui.checkBox_rememberPwd->isChecked();
    json["user_name"] = user_name;
    json["password"] = password;
    QJsonDocument doc;
    doc.setObject(json);
    QFile file("login.json");
    if (file.open(QIODevice::WriteOnly))
    {
        file.write(doc.toJson());
        file.close();
    }
    else
    {
        qDebug() << "打开文件失败";
    }
}