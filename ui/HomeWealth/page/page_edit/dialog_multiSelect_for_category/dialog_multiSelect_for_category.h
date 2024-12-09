#pragma once

#include "Core/core.h" // 确保包含 user_info 定义
#include <QDialog>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>

class dialog_multiSelect_for_category : public QDialog
{
    Q_OBJECT

  public:
    // 修改构造函数，增加 preSelectedUsers 参数
    explicit dialog_multiSelect_for_category(const QVector<category_info> &categoryList, const QVector<category_info> &preSelectedCategory = QVector<category_info>(), QWidget *parent = nullptr);
    ~dialog_multiSelect_for_category();

    QVector<category_info> getSelectedCategory() const;

  private:
    QListWidget *listWidget;
    QPushButton *okButton;
    QPushButton *cancelButton;
    QVector<category_info> selectedCategory;

    // 辅助函数，用于判断用户是否在预选列表中
    bool isCategoryPreSelected(int category_id, const QVector<category_info> &preSelectedCategory) const;
};
