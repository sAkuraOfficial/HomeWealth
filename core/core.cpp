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
    else if (type == "getDataExResponse")
    {
        processGetDataResponse(json);
    }
    else if (type == "updateCategoryDataResponse")
    {
        ProcessUpdateCategoryData(json);
    }
    else if (type == "insertCategoryDataResponse")
    {
        ProcessInsertCategoryData(json);
    }
    else if (type == "deleteCategoryDataResponse")
    {
        ProcessDeleteCategoryData(json);
    }
    else if (type == "getCategorySummaryResponse")
    {
        ProcessGetCategorySummary(json);
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

// void Core::getFriendList(int user_id)
//{
//     QJsonObject json;
//     json["type"] = "getFriendList";
//     json["user_id"] = user_id;
//     QJsonDocument doc(json);
//     m_pProtocol->sendMessage(doc.toJson());
// }

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

void Core::UpdateCategoryData(category_summary newData)
{
    QJsonObject json;
    json["type"] = "updateCategoryData";
    json["category_id"] = newData.category_id;
    json["category_name"] = newData.category_name;
    json["is_income"] = newData.is_income;
    // 无需family_id
    QJsonDocument doc(json);
    m_pProtocol->sendMessage(doc.toJson());
}

void Core::insertCategoryData(category_summary newData, int family_id)
{
    QJsonObject json;
    json["type"] = "insertCategoryData";
    json["category_name"] = newData.category_name;
    json["is_income"] = newData.is_income;
    json["family_id"] = family_id; // 分类所属家庭
    QJsonDocument doc(json);
    m_pProtocol->sendMessage(doc.toJson());
}

void Core::deleteCategoryData(int category_id)
{
    QJsonObject json;
    json["type"] = "deleteCategoryData";
    json["category_id"] = category_id;
    QJsonDocument doc(json);
    m_pProtocol->sendMessage(doc.toJson());
}

void Core::getCategorySummary(int family_id, QVector<int> family_user_id, QVector<int> category_id, searchType type, bool all_time, QDateTime time_A_from, QDateTime time_A_to, QDateTime time_B_from, QDateTime time_B_to)
{
    QJsonObject json;
    json["type"] = "getCategorySummary";
    json["family_id"] = family_id;
    QJsonArray family_user_id_json;
    for (int i = 0; i < family_user_id.size(); i++)
    {
        family_user_id_json.append(family_user_id[i]);
    }
    json["family_user_id"] = family_user_id_json;
    QJsonArray category_id_json;
    for (int i = 0; i < category_id.size(); i++)
    {
        category_id_json.append(category_id[i]);
    }
    json["category_id"] = category_id_json;
    if (type == searchType::all)
        json["search_type"] = "all";
    else if (type == searchType::income)
        json["search_type"] = "income";
    else if (type == searchType::expense)
        json["search_type"] = "expense";
    json["all_time"] = all_time;
    if (!all_time)
    {
        json["time_A_from"] = time_A_from.toString("yyyy-MM-dd hh:mm:ss");
        json["time_A_to"] = time_A_to.toString("yyyy-MM-dd hh:mm:ss");
        json["time_B_from"] = time_B_from.toString("yyyy-MM-dd hh:mm:ss");
        json["time_B_to"] = time_B_to.toString("yyyy-MM-dd hh:mm:ss");
    }
    QJsonDocument doc(json);
    m_pProtocol->sendMessage(doc.toJson());
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

void Core::getDataRequestEx(int user_id, QString keyword, QVector<category_summary> category_list, QVector<user_info> family_user_list, bool search_all_time, searchType type, bool search_time_all, QDateTime start_time, QDateTime end_time)
{
    if (family_user_list.size() == 0)
    {
        Logger::getInstance().log("家庭成员列表为空");
        return;
    }
    QJsonObject json;
    json["type"] = "getDataEx";
    json["keyword"] = keyword;
    json["search_all_time"] = search_all_time;
    if (type == searchType::all)
        json["search_type"] = "all";
    else if (type == searchType::income)
        json["search_type"] = "income";
    else if (type == searchType::expense)
        json["search_type"] = "expense";
    if (search_time_all)
    {
        json["search_time_all"] = search_time_all;
    }
    else
    {
        json["start_time"] = start_time.toString("yyyy-MM-dd hh:mm:ss");
        json["end_time"] = end_time.toString("yyyy-MM-dd hh:mm:ss");
    }
    QJsonArray category_list_json;
    for (int i = 0; i < category_list.size(); i++)
    {
        QJsonObject category;
        category["category_id"] = category_list[i].category_id;
        category_list_json.append(category);
    }
    json["category_list"] = category_list_json;

    QJsonArray family_user_list_json;
    for (int i = 0; i < family_user_list.size(); i++)
    {
        QJsonObject user;
        user["user_id"] = family_user_list[i].user_id;
        family_user_list_json.append(user);
    }
    json["family_user_list"] = family_user_list_json;
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
    emit ReceiveGetDataResponse_normal_or_ex(dataArray);
}

void Core::processGetDataResponseEx(QJsonObject json)
{
    QJsonArray dataArray = json["data"].toArray();
    emit ReceiveGetDataResponse_normal_or_ex(dataArray); // 与普通的数据请求一样
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
    QVector<category_summary> categories;
    for (int i = 0; i < dataArray.size(); i++)
    {
        QJsonObject obj = dataArray[i].toObject();
        category_summary category;
        category.category_id = obj["category_id"].toInt();
        category.category_name = obj["category_name"].toString();
        category.is_income = obj["is_income"].toBool();
        category.amount = obj["amount"].toDouble();
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

void Core::ProcessUpdateCategoryData(QJsonObject json)
{
    bool result = json["result"].toBool();
    if (result)
    {
        emit ReceiveUpdateCategoryData();
    }
    else
    {
        Logger::getInstance().log("更新分类失败");
    }
}

void Core::ProcessInsertCategoryData(QJsonObject json)
{
}

void Core::ProcessDeleteCategoryData(QJsonObject json)
{
}

void Core::ProcessGetCategorySummary(QJsonObject json)
{
    //打印json
    QJsonDocument doc(json);
    Logger::getInstance().log(doc.toJson());
    bool result = json["result"].toBool();
    if (!result)
    {
        QString error = json.contains("error") && json["error"].isString() ? json["error"].toString() : "未知错误";
        Logger::getInstance().log("获取类别摘要失败: " + error, Logger::ERROR);
        return;
    }

    QJsonArray summaryArray = json["summary"].toArray();
    QVector<QVector<category_summary>> summaries(2); // Two QVector for time_A and time_B

    for (const QJsonValue &val : summaryArray)
    {
        if (!val.isObject())
            continue;

        QJsonObject obj = val.toObject();
        category_summary summary;
        summary.category_id = obj["category_id"].toInt();
        summary.category_name = obj["category_name"].toString();
        summary.is_income = obj["is_income"].toBool();

        // Separate A and B time data
        summary.amount = obj["total_amount_A"].toDouble();
        summaries[0].append(summary); // Add to A

        summary.amount = obj["total_amount_B"].toDouble();
        summaries[1].append(summary); // Add to B
    }

    emit ReceiveGetCategorySummary(summaries); // Emit both A and B data
}
