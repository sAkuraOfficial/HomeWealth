#pragma once
#include <QStyledItemDelegate>

// 定义居中代理类
class Delegate_tableItemAlignCenter : public QStyledItemDelegate
{
  public:
    explicit Delegate_tableItemAlignCenter(QObject *parent = nullptr)
        : QStyledItemDelegate(parent)
    {
    }

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        QStyleOptionViewItem opt = option;
        opt.displayAlignment = Qt::AlignCenter; // 设置居中
        QStyledItemDelegate::paint(painter, opt, index);
    }
};