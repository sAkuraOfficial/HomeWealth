#pragma once

#include "ui_dialog_inputCategory.h"
#include <QDialog>
#include <core/core.h>
class dialog_inputCategory : public QDialog
{
    Q_OBJECT

  public:
    dialog_inputCategory(int family_id, Core *core, QWidget *parent = nullptr);
    ~dialog_inputCategory();
  private slots:
    void on_pushButton_ok_clicked();
    void on_pushButton_cancel_clicked();
  private:
    Ui::dialog_inputCategoryClass ui;
    Core *m_core;
    int m_family_id;
};
