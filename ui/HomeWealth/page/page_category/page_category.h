#pragma once

#include "core/core.h"
#include "core/dataTypes.h"
#include "delegate_radio/delegate_radio.h"
#include "dialog_inputCategory/dialog_inputCategory.h"
#include "dialog_multiSelect_for_category/dialog_multiSelect_for_category.h"
#include "dialog_multiSelect_for_familyUser/dialog_multiSelect_for_familyUser.h"
#include "ui_page_category.h"
#include <AnimatedStackedWidget.hpp>
#include <QWidget>
#include <qstandarditemmodel.h>
class page_category : public QWidget
{
    Q_OBJECT

  public:
    page_category(Core *core, user_info user_info, QWidget *parent = nullptr);
    ~page_category();
  private slots:
    void onGetCategory(QVector<category_summary> categories); // 处理获取分类的响应
    void onGetFamilyUserList(QVector<user_info> family_user_list);
    void onItemChanged(QStandardItem *item);
    void on_pushButton_addRow_clicked();
    void on_pushButton_editRow_clicked();
    void on_pushButton_switchPage_to_edit_clicked();
    void on_pushButton_switchPage_to_summary_clicked();
    void on_pushButton_summary_get_data_clicked();
    void on_pushButton_summary_selectFromFamily_clicked();
    void on_pushButton_summary_selectFromCategoryList_clicked();
    void onReceiveGetCategorySummary(QVector<QVector<category_summary>> category_summary_result);
    void on_checkBox_summary_all_family_clicked();
    void on_checkBox_summary_all_category_clicked();
    void onUpdate_UI_under_summary();

  private:
    void setupTableView();
    Ui::page_categoryClass ui;
    Core *m_core;
    QStandardItemModel *model;
    QStandardItemModel *model_A; // 时间范围A的model
    QStandardItemModel *model_B; // 时间范围B的model
    user_info m_user_info;
    QVector<category_summary> m_category_info;
    QVector<user_info> m_family_user_list;

    QVector<category_summary> m_category_info_selected;
    QVector<user_info> m_family_user_list_selected;

    double m_category_edit_income = 0;
    double m_category_edit_expense = 0;
    double m_category_edit_all_income = 0;
    double m_summary_A_edit_income = 0;
    double m_summary_A_edit_expense = 0;
    double m_summary_A_edit_all_income = 0;
    double m_summary_B_edit_income = 0;
    double m_summary_B_edit_expense = 0;
    double m_summary_B_edit_all_income = 0;
  signals:
    void update_UI_under_summary();
};
