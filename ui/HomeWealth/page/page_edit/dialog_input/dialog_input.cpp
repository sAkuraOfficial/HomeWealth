#include "dialog_input.h"

dialog_input::dialog_input(user_info user_info,Core *core, QVector<category_info> category_info, QWidget *parent)
    : QDialog(parent), m_category_info(category_info), m_core(core), m_user_info(user_info)
{
    ui.setupUi(this);
    ui.DateEdit->setDate(QDate::currentDate());
    QVector<QString> categoryNames;
    for (auto category : m_category_info)
    {
        categoryNames.push_back(category.category_name);
    }
    ui.ComboBox->addItems(categoryNames);
}

dialog_input::~dialog_input()
{
}

void dialog_input::on_pushButton_ok_clicked()
{
    QString category_name = ui.ComboBox->currentText();
    QString description = ui.lineEdit_description->text();
    double amount = ui.LineEdit_money->text().toDouble();
    QDate transaction_date = ui.DateEdit->date();
    QString transaction_date_str = transaction_date.toString("yyyy-MM-dd"); // 符合mysql的日期格式
    int category_id = 0;
    for (auto category : m_category_info)
    {
        if (category.category_name == category_name)
        {
            category_id = category.category_id;
            break;
        }
    }
    m_core->insertDetialData(m_user_info.family_id, m_user_info.user_id, category_id, description, amount, transaction_date_str);
    this->close();
}

void dialog_input::on_pushButton_cancel_clicked()
{
    this->close();
}

