#include "dialog_multiSelect_for_familyUser.h"
#include <QHBoxLayout>
#include <QMessageBox>

dialog_multiSelect_for_familyUser::dialog_multiSelect_for_familyUser(const QVector<user_info> &familyUsers, const QVector<user_info> &preSelectedUsers, QWidget *parent)
    : QDialog(parent),
      listWidget(new QListWidget(this)),
      okButton(new QPushButton("确定", this)),
      cancelButton(new QPushButton("取消", this))
{
    setWindowTitle("选择家庭成员");
    resize(300, 400);

    // 设置 QListWidget 支持多选
    listWidget->setSelectionMode(QAbstractItemView::MultiSelection);

    // 添加家庭成员到列表，并根据 preSelectedUsers 设置选中状态
    for (const auto &user : familyUsers)
    {
        QListWidgetItem *item = new QListWidgetItem(user.username, listWidget);
        item->setData(Qt::UserRole, user.user_id); // 存储 user_id 以备后用

        // 检查该用户是否在 preSelectedUsers 中
        for (const auto &preUser : preSelectedUsers)
        {
            if (user.user_id == preUser.user_id)
            {
                item->setSelected(true);
                break;
            }
        }
    }

    // 布局设置
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(listWidget);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);

    mainLayout->addLayout(buttonLayout);

    // 连接信号与槽
    connect(okButton, &QPushButton::clicked, this, [this]() {
        QList<QListWidgetItem *> selectedItems = listWidget->selectedItems();
        if (selectedItems.isEmpty())
        {
            QMessageBox::warning(this, "警告", "请至少选择一个家庭成员。");
            return;
        }

        selectedUsers.clear(); // 清空之前的选择
        for (auto item : selectedItems)
        {
            user_info user;
            user.username = item->text();
            user.user_id = item->data(Qt::UserRole).toInt();
            // 其他字段根据需要设置
            selectedUsers.append(user);
        }
        accept();
    });

    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

dialog_multiSelect_for_familyUser::~dialog_multiSelect_for_familyUser()
{
    // 不需要手动删除子控件，Qt 会自动管理
}

QVector<user_info> dialog_multiSelect_for_familyUser::getSelectedUsers() const
{
    return selectedUsers;
}

bool dialog_multiSelect_for_familyUser::isUserPreSelected(int userId, const QVector<user_info> &preSelectedUsers) const
{
    for (const auto &user : preSelectedUsers)
    {
        if (user.user_id == userId)
            return true;
    }
    return false;
}
