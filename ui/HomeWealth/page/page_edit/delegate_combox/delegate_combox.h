#pragma once

#include <QComboBox>
#include <QStyledItemDelegate>
#include <QWidget>
#include <core/dataTypes.h>

class delegate_combox : public QStyledItemDelegate
{
    Q_OBJECT

  public:
    explicit delegate_combox(QWidget *parent = nullptr);
    ~delegate_combox();

    // 重写父类方法
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    // 重写 paint 方法
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setOptions(const QVector<QString> &options);
    QVector<QString> getOptions() const;

  private:
    QVector<QString> m_options;
};