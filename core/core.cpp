#include "core.h"
#include "Logger/logger.h"

Core::Core(QObject *parent)
    : QObject(parent)
{
    m_pProtocol = new Protocol();

    // 初始化定时器
    timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(m_pProtocol->getWebSocket(), &QWebSocket::connected, this, [=]() {
        timer->stop();
        emit ConnectSuccess();
    });
    connect(timer, &QTimer::timeout, this, [=]() {
        m_pProtocol->getWebSocket()->close(); // 超时关闭连接
        emit ConnectTimeOut();
    });
    connect(m_pProtocol, &Protocol::ReceiveNewMessage, this, &Core::onReceiveNewMessage); // 接收到新消息，然后路由转发到对应处理函数
    connect(m_pProtocol->getWebSocket(), &QWebSocket::disconnected, this, [=]() {
        emit signal_disconnect();
    });
}

Core::~Core()
{
}

Protocol *Core::getProtocol()
{
    return m_pProtocol;
}

bool Core::isConnecting()
{
    return m_pProtocol->getWebSocket()->state() == QAbstractSocket::SocketState::ConnectingState;
}

void Core::runClient(QString ws, int timeoutMs)
{
    m_pProtocol->ConnectToServer(ws);
    timer->start(timeoutMs);
    emit beginConnect();
}

void Core::setClientUserInfo(user_info *client_user_info)
{
    m_client_user_info = client_user_info;
}

void Core::onReceiveNewMessage(QString message)
{
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());
    // Logger::getInstance().log("\n"+ doc.toJson());
    QJsonObject json = doc.object();
    QString type = json["type"].toString();
    if (type == "login")
    {
        processLogin(json);
    }
    else if (type == "register")
    {
        processRegister(json);
    }
    else if (type == "getDataResponse")
    {
        processGetDataResponse(json);
    }
    else if (type == "insertDetialDataResponse")
    {
        ProcessInsertDetialData(json);
    }
    else if (type == "getCategoryResponse")
    {
        ProcessGetCategory(json);
    }
    else if (type == "deleteDetialDataResponse")
    {
        ProcessDeleteDetialData(json);
    }
    else if (type == "getFamilyUserListResponse")
    {
        ProcessGetFamilyUserList(json);
    }
    else
    {
        Logger::getInstance().log("未知消息类型");
    }
}

//-----------------------------------------------------发送消息-----------------------------------------------------

void Core::login(QString username, QString password)
{
    QJsonObject json;
    json["type"] = "login";
    json["username"] = username;
    json["password"] = password;
    QJsonDocument doc(json);
    m_pProtocol->sendMessage(doc.toJson());
}

void Core::registerUser(QString username, QString password)
{
    QJsonObject json;
    json["type"] = "register";
    json["username"] = username;
    json["password"] = password;
    QJsonDocument doc(json);
    m_pProtocol->sendMessage(doc.toJson());
}

void Core::getFriendList(int user_id)
{
    QJsonObject json;
    json["type"] = "getFriendList";
    json["user_id"] = user_id;
    QJsonDocument doc(json);
    m_pProtocol->sendMessage(doc.toJson());
}

void Core::getDataRequest(int user_id)
{
    QJsonObject json;
    json["type"] = "getData";
    json["user_id"] = user_id;
    QJsonDocument doc(json);
    m_pProtocol->sendMessage(doc.toJson());
}

void Core::UpdateDetialData(int detail_id, QString column_name, QVariant newValue)
{
    QJsonObject json;
    json["type"] = "updateDetialData";
    json["detail_id"] = detail_id;
    json["column_name"] = column_name;
    json["value"] = QJsonValue::fromVariant(newValue);
    QJsonDocument doc(json);
    m_pProtocol->sendMessage(doc.toJson(QJsonDocument::Compact));
}

void Core::insertDetialData(int family_id, int user_id, int category_id, QString description, double amount, QString transaction_date)
{
    QJsonObject json;
    json["type"] = "insertDetialData";
    json["family_id"] = family_id;
    json["user_id"] = user_id;
    json["category_id"] = category_id;
    json["description"] = description;
    json["amount"] = amount;
    json["transaction_date"] = transaction_date;
    QJsonDocument doc(json);
    m_pProtocol->sendMessage(doc.toJson());
}

void Core::getCategory(int family_id)
{
    QJsonObject json;
    json["type"] = "getCategory";
    json["family_id"] = family_id;
    QJsonDocument doc(json);
    m_pProtocol->sendMessage(doc.toJson());
}

void Core::deleteDetialData(int detail_id)
{
    QJsonObject json;
    json["type"] = "deleteDetialData";
    json["detail_id"] = detail_id;
    QJsonDocument doc(json);
    m_pProtocol->sendMessage(doc.toJson());
}

void Core::getFamilyUserList(int family_id)
{
    QJsonObject json;
    json["type"] = "getFamilyUserList";
    json["family_id"] = family_id;
    QJsonDocument doc(json);
    m_pProtocol->sendMessage(doc.toJson());
}

//-----------------------------------------------------处理接收的消息-----------------------------------------------------

void Core::processLogin(QJsonObject msg_json)
{
    bool result = msg_json["result"].toBool();
    int user_id = result ? msg_json["user_id"].toInt() : -1;
    QString username = result ? msg_json["username"].toString() : "无法获取用户名";
    int family_id = result ? msg_json["family_id"].toInt() : -1;   // 登录成功且-1,代表没有家庭。登录失败为-1
    bool isAdmin = result ? msg_json["is_admin"].toBool() : false; // 登录成功且false,代表不是管理员。登录失败为false
    emit ReceiveLoginResult(result, username, user_id, isAdmin, family_id);
}

void Core::processRegister(QJsonObject msg_json)
{
}

void Core::processGetDataResponse(QJsonObject json)
{
    QJsonArray dataArray = json["data"].toArray();
    emit dataReceived(dataArray);
}

void Core::ProcessInsertDetialData(QJsonObject json)
{
    bool result = json["result"].toBool();
    // 若出错才报错
    if (json["result"] == false)
    {
        Logger::getInstance().log("插入数据失败");
        emit insertDetialDataError();
    }
    else
    {
        Logger::getInstance().log("插入数据成功");
    }
}

void Core::ProcessGetCategory(QJsonObject json)
{
    QJsonArray dataArray;
    if (json["result"].toBool())
    {
        dataArray = json["data"].toArray();
    }
    QVector<category_info> categories;
    for (int i = 0; i < dataArray.size(); i++)
    {
        QJsonObject obj = dataArray[i].toObject();
        category_info category;
        category.category_id = obj["category_id"].toInt();
        category.category_name = obj["category_name"].toString();
        category.is_income = obj["is_income"].toBool();
        categories.push_back(category);
    }
    emit ReceiveGetCategory(categories);
}

void Core::ProcessDeleteDetialData(QJsonObject json)
{
    bool result = json["result"].toBool();
    if (result)
    {
        emit ReceiveDeleteDetialData();
    }
    else
    {
        Logger::getInstance().log("删除数据失败");
    }
}

void Core::ProcessGetFamilyUserList(QJsonObject json)
{
    QJsonArray dataArray;
    if (json["result"].toBool())
    {
        dataArray = json["data"].toArray();
    }
    QVector<user_info> users;
    for (int i = 0; i < dataArray.size(); i++)
    {
        QJsonObject obj = dataArray[i].toObject();
        user_info user;
        user.user_id = obj["user_id"].toInt();
        user.username = obj["login_name"].toString();
        user.family_id = obj["family_id"].toInt();
        user.isAdmin = obj["is_admin"].toBool();
        users.push_back(user);
    }
    emit ReceiveGetFamilyUserList(users);
}
