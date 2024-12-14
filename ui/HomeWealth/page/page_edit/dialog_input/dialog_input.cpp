#include "dialog_input.h"

dialog_input::dialog_input(user_info user_info, Core *core, QVector<category_summary> category_info, QWidget *parent)
    : QDialog(parent), m_category_info(category_info), m_core(core), m_user_info(user_info)
{
    ui.setupUi(this);
    ui.DateEdit->setDate(QDate::currentDate());
    // 设置radio的默认值
    ui.radioButton_expense->setChecked(true);
    ui.radioButton_income->setChecked(false);
    m_is_income = false;

    QVector<QString> categoryNames;
    for (auto category : m_category_info)
    {
        if (category.is_income == m_is_income)
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

void dialog_input::on_radioButton_expense_clicked()
{
    if (m_is_income == false)
    {
        // 已经是支出了
        return;
    }
    else
    {
        // 状态从income转换为expense
        m_is_income = false;
        ui.ComboBox->clear();
        QVector<QString> categoryNames;
        for (auto category : m_category_info)
        {
            if (category.is_income == m_is_income)
                categoryNames.push_back(category.category_name);
        }
        ui.ComboBox->addItems(categoryNames);

    }
}

void dialog_input::on_radioButton_income_clicked()
{
    if (m_is_income==true)
    {
        // 已经是收入了
        return;
    }
    else
    {
        // 状态从expense转换为income
        m_is_income = true;
        ui.ComboBox->clear();
        QVector<QString> categoryNames;
        for (auto category : m_category_info)
        {
            if (category.is_income == m_is_income)
                categoryNames.push_back(category.category_name);
        }
        ui.ComboBox->addItems(categoryNames);
    }
}
