// delegate_combox.cpp
#include "delegate_combox.h"
#include <QComboBox>
#include <QPainter>
#include <QStyle>
#include <QStyleOptionComboBox>
#include <qapplication.h>
delegate_combox::delegate_combox(QWidget *parent)
    : QStyledItemDelegate(parent)
{
}

delegate_combox::~delegate_combox()
{
}

QWidget *delegate_combox::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QComboBox *editor = new QComboBox(parent);
    editor->addItems(m_options);
    // 设置当前内容与模型中的内容一致
    editor->setCurrentText(index.model()->data(index, Qt::EditRole).toString());
    return editor;
}

void delegate_combox::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QComboBox *comboBox = qobject_cast<QComboBox *>(editor);
    if (comboBox)
    {
        QString currentText = index.model()->data(index, Qt::EditRole).toString();
        int currentIndex = comboBox->findText(currentText);
        if (currentIndex >= 0)
            comboBox->setCurrentIndex(currentIndex);
    }
}

void delegate_combox::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox *comboBox = qobject_cast<QComboBox *>(editor);
    if (comboBox)
    {
        model->setData(index, comboBox->currentText(), Qt::EditRole);
    }
}

void delegate_combox::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    editor->setGeometry(option.rect);
}

void delegate_combox::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyledItemDelegate::paint(painter, option, index);
}

void delegate_combox::setOptions(const QVector<QString> &options)
{
    m_options = options;
}

QVector<QString> delegate_combox::getOptions() const
{
    return m_options;
}
