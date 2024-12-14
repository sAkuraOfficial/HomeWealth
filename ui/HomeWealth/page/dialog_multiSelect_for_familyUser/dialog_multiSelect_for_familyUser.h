#pragma once

#include "Core/core.h" // 确保包含 user_info 定义
#include <QDialog>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>

class dialog_multiSelect_for_familyUser : public QDialog
{
    Q_OBJECT

  public:
    // 修改构造函数，增加 preSelectedUsers 参数
    explicit dialog_multiSelect_for_familyUser(const QVector<user_info> &familyUsers, const QVector<user_info> &preSelectedUsers = QVector<user_info>(), QWidget *parent = nullptr);
    ~dialog_multiSelect_for_familyUser();

    QVector<user_info> getSelectedUsers() const;

  private:
    QListWidget *listWidget;
    QPushButton *okButton;
    QPushButton *cancelButton;
    QVector<user_info> selectedUsers;

    // 辅助函数，用于判断用户是否在预选列表中
    bool isUserPreSelected(int userId, const QVector<user_info> &preSelectedUsers) const;
};
