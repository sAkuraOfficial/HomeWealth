#include "dialog_insertUser.h"

dialog_insertUser::dialog_insertUser(Core *core, user_info user, QWidget *parent)
    : QDialog(parent), m_core(core), m_user_info(user)
{
    ui.setupUi(this);
    ui.radioButton_create->setChecked(true);
    ui.radioButton_have->setChecked(false);
    ui.LineEdit_loginname->setEnabled(true);
    ui.LineEdit_pwd->setEnabled(true);
    ui.LineEdit_id->setEnabled(false);
    connect(m_core, &Core::ReceiveInsertUserToFamily, this, &dialog_insertUser::onReceiveInsertUserToFamily_And_ReceiveCreateAndInsertUserToFamily);
    connect(m_core, &Core::ReceiveCreateAndInsertUserToFamily, this, &dialog_insertUser::onReceiveInsertUserToFamily_And_ReceiveCreateAndInsertUserToFamily);
}

dialog_insertUser::~dialog_insertUser()
{
}

void dialog_insertUser::on_pushButton_ok_clicked()
{
    if (ui.radioButton_create->isChecked())
    {
        QString loginname = ui.LineEdit_loginname->text();
        QString pwd = ui.LineEdit_pwd->text();
        m_core->createInsertUserToFamily(m_user_info.family_id, loginname, pwd);
    }
    else
    {
        QString user_name = ui.LineEdit_id->text();
        m_core->insertUserToFamily(m_user_info.family_id, user_name);
    }
}

void dialog_insertUser::on_pushButton_cancel_clicked()
{
    this->close();
}

void dialog_insertUser::on_radioButton_create_clicked()
{
    ui.LineEdit_loginname->setEnabled(true);
    ui.LineEdit_pwd->setEnabled(true);
    ui.LineEdit_id->setEnabled(false);
}

void dialog_insertUser::on_radioButton_have_clicked()
{
    ui.LineEdit_loginname->setEnabled(false);
    ui.LineEdit_pwd->setEnabled(false);
    ui.LineEdit_id->setEnabled(true);
}

void dialog_insertUser::onReceiveInsertUserToFamily_And_ReceiveCreateAndInsertUserToFamily(bool success, QString error_msg)
{
    m_core->getFamilyUserList(m_user_info.family_id);
    if (success)
    {
        QMessageBox::information(this, "成功", "添加成功");
        this->close();
    }
    else
    {
        QMessageBox::warning(this, "失败", error_msg);
    }
}
