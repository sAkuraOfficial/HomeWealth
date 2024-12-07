#pragma once

#include "ui_dialog_input.h"
#include <QDialog>
#include <core/core.h>
#include <core/dataTypes.h>
#include <qdatetime.h>


class dialog_input : public QDialog
{
    Q_OBJECT

  public:
    dialog_input(user_info user_info,Core *core, QVector<category_info> category_info, QWidget *parent = nullptr);
    ~dialog_input();

  private:
    Core *m_core;
    user_info m_user_info;
    Ui::dialog_inputClass ui;
    QVector<category_info> m_category_info;

  private slots:
    void on_pushButton_ok_clicked();
    void on_pushButton_cancel_clicked();
};
