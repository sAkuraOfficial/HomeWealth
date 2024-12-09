#include "dialog_inputCategory.h"

dialog_inputCategory::dialog_inputCategory(int family_id, Core *core, QWidget *parent)
    : QDialog(parent), m_core(core), m_family_id(family_id)
{
    ui.setupUi(this);
}

dialog_inputCategory::~dialog_inputCategory()
{
}
void dialog_inputCategory::on_pushButton_ok_clicked()
{
    category_info category_info;
    category_info.category_name = ui.LineEdit_category_name->text();
    category_info.is_income = ui.radioButton_income->isChecked();
    m_core->insertCategoryData(category_info, m_family_id);
    this->close();
}

void dialog_inputCategory::on_pushButton_cancel_clicked()
{
    this->close();
}
