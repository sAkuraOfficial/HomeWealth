#pragma once

#include "core/core.h"
#include "core/dataTypes.h"
#include "delegate_radio/delegate_radio.h"
#include "ui_page_category.h"
#include <QWidget>
#include <qstandarditemmodel.h>
#include"dialog_inputCategory/dialog_inputCategory.h"
class page_category : public QWidget
{
    Q_OBJECT

  public:
    page_category(Core *core, user_info user_info, QWidget *parent = nullptr);
    ~page_category();
  private slots:
    void onGetCategory(QVector<category_info> categories); // 处理获取分类的响应
    void onItemChanged(QStandardItem *item);
    void on_pushButton_addRow_clicked();
    void on_pushButton_editRow_clicked();
  private:
    void setupTableView();
    Ui::page_categoryClass ui;
    Core *m_core;
    QStandardItemModel *model;
    user_info m_user_info;
};
