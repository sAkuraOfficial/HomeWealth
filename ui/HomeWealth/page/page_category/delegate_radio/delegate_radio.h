#pragma once
#include <QPainter>
#include <QRadioButton>
#include <QStyledItemDelegate>

class delegate_radio : public QStyledItemDelegate
{
    Q_OBJECT
  public:
    explicit delegate_radio(QObject *parent = nullptr);


    // 重写编辑事件
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    void setEditorData(QWidget *editor, const QModelIndex &index) const override;

    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;

    // 重写更新编辑器位置和大小
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};