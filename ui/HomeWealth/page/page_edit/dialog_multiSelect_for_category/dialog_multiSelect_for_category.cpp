#include "dialog_multiSelect_for_category.h"
#include <QHBoxLayout>
#include <QMessageBox>

dialog_multiSelect_for_category::dialog_multiSelect_for_category(const QVector<category_info> &categoryList, const QVector<category_info> &preSelectedCategory , QWidget *parent)
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
    for (const auto &category : categoryList)
    {
        QListWidgetItem *item = new QListWidgetItem(category.category_name, listWidget);
        item->setData(Qt::UserRole, category.category_id); // 存储 user_id 以备后用

        // 检查该用户是否在 preSelectedUsers 中
        for (const auto &preCategory : preSelectedCategory)
        {
            if (category.category_id == preCategory.category_id)
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

        selectedCategory.clear(); // 清空之前的选择
        for (auto item : selectedItems)
        {
            category_info category;
            category.category_name = item->text();
            category.category_id = item->data(Qt::UserRole).toInt();
            // 其他字段根据需要设置
            selectedCategory.append(category);
        }
        accept();
    });

    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

dialog_multiSelect_for_category::~dialog_multiSelect_for_category()
{
    // 不需要手动删除子控件，Qt 会自动管理
}

bool dialog_multiSelect_for_category::isCategoryPreSelected(int category_id, const QVector<category_info> &preSelectedCategory) const
{
    for (const auto &category : preSelectedCategory)
    {
        if (category.category_id == category_id)
            return true;
    }
    return false;
}

QVector<category_info> dialog_multiSelect_for_category::getSelectedCategory() const
{
    return selectedCategory;
}
