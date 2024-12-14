#pragma once
#include <QByteArray>
#include <qdatetime.h>
#include <qstring.h>
#include <qwebsocket.h>

struct user_info
{
    QString username;
    int user_id;
    int family_id;
    bool isAdmin;
    QByteArray avatar;
};
enum class searchType
{
    all,    // 全部收入支出
    income, // 收入
    expense // 支出
};


struct client_info : public user_info
{
    QWebSocket *socket;
};


struct category_summary
{
    int category_id;
    QString category_name;
    bool is_income;
    double amount;
};

struct family_info
{
    int family_id;
    QString family_name;
};