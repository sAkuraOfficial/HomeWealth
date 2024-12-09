// delegate_radio.cpp
#include "delegate_radio.h"
#include <QApplication>
#include <QEvent>
#include <QHBoxLayout>
#include <QMouseEvent>

delegate_radio::delegate_radio(QObject *parent)
    : QStyledItemDelegate(parent)
{
}


QWidget *delegate_radio::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // 创建一个无边框的控件作为编辑器
    QWidget *editor = new QWidget(parent);
    QHBoxLayout *layout = new QHBoxLayout(editor);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addStretch();

    QRadioButton *incomeRadio = new QRadioButton("收入", editor);
    QRadioButton *expenseRadio = new QRadioButton("支出", editor);

    // 设置对象名称以便后续查找
    incomeRadio->setObjectName("incomeRadio");
    expenseRadio->setObjectName("expenseRadio");

    layout->addWidget(incomeRadio);
    layout->addWidget(expenseRadio);
    layout->addStretch();

    return editor;
}

void delegate_radio::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QString value = index.model()->data(index, Qt::DisplayRole).toString();
    QRadioButton *incomeRadio = editor->findChild<QRadioButton *>("incomeRadio");
    QRadioButton *expenseRadio = editor->findChild<QRadioButton *>("expenseRadio");

    if (value == "收入")
    {
        incomeRadio->setChecked(true);
    }
    else
    {
        expenseRadio->setChecked(true);
    }
}

void delegate_radio::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QRadioButton *incomeRadio = editor->findChild<QRadioButton *>("incomeRadio");
    QRadioButton *expenseRadio = editor->findChild<QRadioButton *>("expenseRadio");

    if (incomeRadio->isChecked())
    {
        model->setData(index, "收入", Qt::EditRole);
    }
    else
    {
        model->setData(index, "支出", Qt::EditRole);
    }
}

void delegate_radio::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    editor->setGeometry(option.rect);
}