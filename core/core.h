#pragma once
// 客户端内核类,作为protocol与ui的中间层
#include "core/protocol.h"
#include <QEventLoop>
#include <core/dataTypes.h>
#include <qjsonarray.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qobject.h>
#include <qtimer.h>
#include <qwebsocket.h>
class Core : public QObject
{
    Q_OBJECT
  public:
    Core(QObject *parent = nullptr);
    ~Core();
    Protocol *getProtocol();
    bool isConnecting();
    void setClientUserInfo(user_info *client_user_info); // 仅可由ClientMain设置
    void runClient(QString ws, int timeoutMs);           // 第二个参数：连接超时时间

    void login(QString username, QString password);
    void registerUser(QString username, QString password);
    // void getFriendList(int user_id);
    //  项目分类的操作
    void getCategory(int family_id);
    void UpdateCategoryData(category_summary newData);
    void insertCategoryData(category_summary newData, int family_id);
    void deleteCategoryData(int category_id);
    void getCategorySummary(
        int family_id,
        QVector<int> family_user_id,
        QVector<int> category_id,
        searchType type,
        bool all_time,
        QDateTime time_A_from,
        QDateTime time_A_to,
        QDateTime time_B_from,
        QDateTime time_B_to
    );
    // 明细数据的操作
    void getDataRequest(int user_id); // 请求数据
    void UpdateDetialData(int detail_id, QString column_name, QVariant newValue);
    void insertDetialData(int family_id, int user_id, int category_id, QString description, double amount, QString transaction_date);
    void deleteDetialData(int detail_id);
    void getDataRequestEx(
        int user_id,
        QString keyword,
        QVector<category_summary> category_list,
        QVector<user_info> family_user_list,
        bool search_all_time,
        searchType type,
        bool search_time_all,
        QDateTime start_time = QDateTime(),
        QDateTime end_time = QDateTime()
    );
    // 家庭成员的操作
    void getFamilyUserList(int family_id);
    void deleteUserFromFamily(int family_id, int user_id);
    void insertUserToFamily(int family_id, int user_id);                                   // 把已有用户加入家庭
    void createInsertUserToFamily(int family_id, QString username, QString password); // 新建用户并插入家庭
    // void sendMessage(message_info message);
    // 处理接收到的消息,msg是接收到的json消息
    void processLogin(QJsonObject msg_json);
    void processRegister(QJsonObject msg_json);
    // void processGetFriendList(QJsonObject msg_json);
    // void processNewMessage(QJsonObject msg_json);

  private:
    Protocol *m_pProtocol = nullptr;
    QTimer *timer = nullptr;
    user_info *m_client_user_info;
    void processGetDataResponse(QJsonObject json); // 处理数据响应
    void processGetDataResponseEx(QJsonObject json);
    void ProcessInsertDetialData(QJsonObject json);
    void ProcessGetCategory(QJsonObject json);
    void ProcessDeleteDetialData(QJsonObject json);
    void ProcessGetFamilyUserList(QJsonObject json);
    void ProcessUpdateCategoryData(QJsonObject json);
    void ProcessInsertCategoryData(QJsonObject json);
    void ProcessDeleteCategoryData(QJsonObject json);
    void ProcessGetCategorySummary(QJsonObject json);
    void ProcessInsertUserToFamily(QJsonObject json);
    void ProcessCreateInsertUserToFamily(QJsonObject json);
    void ProcessDeleteUserFromFamily(QJsonObject json);
  private slots:
    void onReceiveNewMessage(QString message);
  signals:
    void ReceiveGetDataResponse_normal_or_ex(QJsonArray dataArray);                                                           // 当接收到数据时发射
    void beginConnect();                                                                               // 发送开始连接信号，用于ui显示加载动画
    void ConnectTimeOut();                                                                             // 连接超时
    void ConnectSuccess();                                                                             // 连接成功
    void ReceiveLoginResult(bool result, QString username, int user_id, bool is_admin, int family_id); // 登录结果
    void ReceiveRegisterResult(bool result);                                                           // 注册结果
    void insertDetialDataError();                                                                      // 插入数据失败
    void signal_disconnect();                                                                          // 断开连接
    void ReceiveGetCategory(QVector<category_summary> categories);                                     // 获取分类，提供给page_edit与page_category使用
    void ReceiveDeleteDetialData();                                                                    // 删除数据
    void ReceiveGetFamilyUserList(QVector<user_info> users);                                           // 获取家庭成员
    void ReceiveUpdateCategoryData();                                                                  // 更新分类
    void ReceiveInsertCategoryData();                                                                  // 插入分类
    void ReceiveDeleteCategoryData();                                                                  // 删除分类
    void ReceiveGetCategorySummary(QVector<QVector<category_summary>>);                                // 获取分类汇总
    void ReceiveInsertUserToFamily(bool success,QString error_msg);                                                              // 插入家庭成员
    void ReceiveCreateAndInsertUserToFamily(bool success, QString error_msg);                    // 创建并插入家庭成员
    void ReceiveDeleteUserFromFamily(bool success, QString error_msg);                                                           // 删除家庭成员

    // void ReceiveGetFriendList(QVector<friend_info> friends);
    // void ReceiveUserMessage(message_info message);
};

// 数据库结构
/*
SET FOREIGN_KEY_CHECKS = 0;

-- ----------------------------
-- Table structure for family
-- ----------------------------
DROP TABLE IF EXISTS `family`;
CREATE TABLE `family` (
  `family_id` INT NOT NULL AUTO_INCREMENT,
  `family_name` VARCHAR(255) NOT NULL,
  PRIMARY KEY (`family_id`)
);

-- ----------------------------
-- Table structure for user
-- ----------------------------
DROP TABLE IF EXISTS `user`;
CREATE TABLE `user` (
  `user_id` INT NOT NULL AUTO_INCREMENT,
  `login_name` VARCHAR(255) NOT NULL,
  `password` VARCHAR(255) NOT NULL,
  PRIMARY KEY (`user_id`)
);

-- ----------------------------
-- Table structure for family_user (middle table)
-- ----------------------------
DROP TABLE IF EXISTS `family_user`;
CREATE TABLE `family_user` (
  `family_id` INT NOT NULL,
  `user_id` INT NOT NULL,
  `is_admin` BOOLEAN NOT NULL DEFAULT FALSE,
  PRIMARY KEY (`family_id`, `user_id`),
  FOREIGN KEY (`family_id`) REFERENCES `family` (`family_id`) ON DELETE CASCADE ON UPDATE CASCADE,
  FOREIGN KEY (`user_id`) REFERENCES `user` (`user_id`) ON DELETE CASCADE ON UPDATE CASCADE
);

DROP TABLE IF EXISTS `category`;
CREATE TABLE `category` (
  `category_id` INT NOT NULL AUTO_INCREMENT,
  `category_name` VARCHAR(255) NOT NULL,
  `is_income` BOOLEAN NOT NULL DEFAULT FALSE, -- FALSE: 支出, TRUE: 收入
  `family_id` INT NOT NULL,
  PRIMARY KEY (`category_id`),
  FOREIGN KEY (`family_id`) REFERENCES `family` (`family_id`) ON DELETE CASCADE ON UPDATE CASCADE
);

DROP TABLE IF EXISTS `transaction_detail`;
CREATE TABLE `transaction_detail` (
  `detail_id` INT NOT NULL AUTO_INCREMENT,
  `family_id` INT NOT NULL,
  `user_id` INT NOT NULL,
  `category_id` INT NOT NULL, -- 项目分类
  `description` VARCHAR(255) NULL, -- 明细内容
  `amount` DECIMAL(10, 2) NOT NULL, -- 支出为负，收入为正
  `transaction_date` DATETIME NOT NULL, -- 记录明细的日期
  PRIMARY KEY (`detail_id`),
  FOREIGN KEY (`family_id`) REFERENCES `family` (`family_id`) ON DELETE CASCADE ON UPDATE CASCADE,
  FOREIGN KEY (`user_id`) REFERENCES `user` (`user_id`) ON DELETE CASCADE ON UPDATE CASCADE,
  FOREIGN KEY (`category_id`) REFERENCES `category` (`category_id`) ON DELETE CASCADE ON UPDATE CASCADE
);


SET FOREIGN_KEY_CHECKS = 1;

-- 插入家庭数据
INSERT INTO `family` (`family_name`) VALUES
('family_a'),
('family_b'),
('family_c');

-- 插入用户数据
INSERT INTO `user` (`login_name`, `password`) VALUES
('a', 'a'),
('b', 'b'),
('c', 'c'),
('d', 'd'),
('e', 'e'),
('f', 'f'),
('g', 'g'),
('h', 'h'),
('i', 'i'),
('j', 'j'),
('k', 'k'),
('l', 'l'),
('m', 'm'),
('n', 'n'),
('o', 'o');

-- 插入家庭成员关系数据
-- Family A: a, b, c, d, e (a 是管理员)
INSERT INTO `family_user` (`family_id`, `user_id`, `is_admin`) VALUES
(1, 1, TRUE), -- a 是 family_a 的管理员
(1, 2, FALSE), -- b
(1, 3, FALSE), -- c
(1, 4, FALSE), -- d
(1, 5, FALSE); -- e

-- Family B: f, g, h, i, j (f 是管理员)
INSERT INTO `family_user` (`family_id`, `user_id`, `is_admin`) VALUES
(2, 6, TRUE), -- f 是 family_b 的管理员
(2, 7, FALSE), -- g
(2, 8, FALSE), -- h
(2, 9, FALSE), -- i
(2, 10, FALSE); -- j

-- Family C: k, l, m, n, o (k 是管理员)
INSERT INTO `family_user` (`family_id`, `user_id`, `is_admin`) VALUES
(3, 11, TRUE), -- k 是 family_c 的管理员
(3, 12, FALSE), -- l
(3, 13, FALSE), -- m
(3, 14, FALSE), -- n
(3, 15, FALSE); -- o

-- 插入分类数据
-- Family A (family_id = 1) 的分类
INSERT INTO `category` (`category_name`, `is_income`, `family_id`) VALUES
('工资', TRUE, 1),
('餐饮', FALSE, 1),
('娱乐', FALSE, 1);

-- Family B (family_id = 2) 的分类
INSERT INTO `category` (`category_name`, `is_income`, `family_id`) VALUES
('奖金', TRUE, 2),
('购物', FALSE, 2),
('交通', FALSE, 2);

-- Family C (family_id = 3) 的分类
INSERT INTO `category` (`category_name`, `is_income`, `family_id`) VALUES
('投资收益', TRUE, 3),
('房租', FALSE, 3),
('教育', FALSE, 3);

-- 插入支出/收入明细数据
-- Family A (family_id = 1) 的明细
INSERT INTO `transaction_detail` (`family_id`, `user_id`, `category_id`, `description`, `amount`, `transaction_date`) VALUES
(1, 1, 1, '6月工资', 5000.00, '2024-06-01 10:00:00'),
(1, 2, 2, '聚餐', -200.00, '2024-06-02 19:30:00'),
(1, 3, 3, '电影票', -50.00, '2024-06-03 20:00:00');

-- Family B (family_id = 2) 的明细
INSERT INTO `transaction_detail` (`family_id`, `user_id`, `category_id`, `description`, `amount`, `transaction_date`) VALUES
(2, 6, 4, '季度奖金', 3000.00, '2024-06-10 15:00:00'),
(2, 7, 5, '买衣服', -500.00, '2024-06-11 14:00:00'),
(2, 8, 6, '地铁票', -20.00, '2024-06-12 08:30:00');

-- Family C (family_id = 3) 的明细
INSERT INTO `transaction_detail` (`family_id`, `user_id`, `category_id`, `description`, `amount`, `transaction_date`) VALUES
(3, 11, 7, '股票收益', 2000.00, '2024-06-15 10:00:00'),
(3, 12, 8, '支付房租', -1500.00, '2024-06-16 12:00:00'),
(3, 13, 9, '买书', -100.00, '2024-06-17 10:00:00');


*/