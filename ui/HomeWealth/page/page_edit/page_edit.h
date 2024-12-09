#pragma once

#include "./dialog_input/dialog_input.h"
#include "Core/core.h"
#include "dialog_multiSelect_for_familyUser/dialog_multiSelect_for_familyUser.h"
#include "dialog_multiSelect_for_category/dialog_multiSelect_for_category.h"
#include "ui_page_edit.h"
#include <QStandardItemModel>
#include <QWidget>

class page_edit : public QWidget
{
    Q_OBJECT

  public:
    explicit page_edit(Core *core, user_info user_info, QWidget *parent = nullptr);
    ~page_edit();

  private slots:
    void populateTable(QJsonArray dataArray);
    void onItemChanged(QStandardItem *item);
    void onAddRowClicked();                                               // 新增槽函数
    void onInsertDataResponse(QJsonObject json);                          // 处理插入数据的响应
    void onGetCategory(QVector<category_info> categories);                // 处理获取分类的响应
    void onReceiveGetFamilyUserList(QVector<user_info> family_user_list); // 接收家庭成员列表
    void on_pushButton_deleteRow_clicked();                               // 删除行
    void on_pushButton_refresh_clicked();                                 // 刷新
    void on_checkBox_searchUser_selectMe_clicked();                       // 选择只查询自己
    void on_checkBox_searchTime_all_clicked();                            // 选择查询所有时间
    void on_pushButton_searchUser_selectFromFamily_clicked();             // 点击按钮查询家庭成员
    void on_pushButton_searchCategory_selectFromCategoryList_clicked(); // 点击按钮查询分类
    void on_toolButton_search_clicked();
    void onRadioButtonClicked();
    void on_lineEdit_search_keyword_textChanged();
    void on_checkBox_searchCategory_selectAllCategory_clicked();
  private:
    Ui::page_editClass ui;
    Core *m_core;
    QStandardItemModel *model;
    // family_info m_family_info;
    user_info m_user_info;
    QVector<user_info> m_family_user_list;
    QVector<user_info> m_selected_family_user;
    QVector<category_info> m_category_info;
    QVector<category_info> m_selected_category;


    void setupTableView();
};
