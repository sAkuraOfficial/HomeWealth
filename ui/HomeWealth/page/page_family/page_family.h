#pragma once

#include "ui_page_family.h"
#include <QWidget>
#include <core/core.h>
#include <core/dataTypes.h>
#include <qstandarditemmodel.h>
#include <qtableview.h>
#include"delegate_tableItemAlignCenter/delegate_tableItemAlignCenter.hpp"
class page_family : public QWidget
{
    Q_OBJECT

  public:
    page_family(Core *core, user_info user, QWidget *parent = nullptr);
    ~page_family();
  private slots:
    void on_pushButton_addRow_clicked();
    void on_pushButton_deleteRow_clicked();
    void on_pushButton_refresh_clicked();
    void onReceiveGetFamilyUserList(QVector<user_info> family_user_list);
    void onReceiveDeleteUserFromFamily(bool result, QString error_msg);
  private:
    Ui::page_familyClass ui;
    Core *m_core;
    user_info m_user_info;
    QVector<user_info> m_family_user_list;
    QStandardItemModel *model;

    void setupTable();
};
