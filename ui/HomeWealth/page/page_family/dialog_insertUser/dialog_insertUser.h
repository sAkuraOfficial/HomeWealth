#pragma once

#include <QDialog>
#include "ui_dialog_insertUser.h"
#include<core/core.h>
#include<core/dataTypes.h>
class dialog_insertUser : public QDialog
{
	Q_OBJECT

public:
    dialog_insertUser(Core *core, user_info user, QWidget *parent = nullptr);
	~dialog_insertUser();
  private slots:
    void on_pushButton_ok_clicked();
    void on_pushButton_cancel_clicked();
    void on_radioButton_create_clicked();
    void on_radioButton_have_clicked();
    void onReceiveInsertUserToFamily_And_ReceiveCreateAndInsertUserToFamily(bool success, QString error_msg);

  private:
	Ui::dialog_insertUserClass ui;
  Core *m_core;
    user_info m_user_info;
};
