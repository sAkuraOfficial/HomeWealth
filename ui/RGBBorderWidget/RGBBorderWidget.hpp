#pragma once

#include <QColor>
#include <QPainter>
#include <QTimer>
#include <QWidget>

class RGBBorderWidget : public QWidget
{
    Q_OBJECT

  public:
    explicit RGBBorderWidget(QWidget *parent = nullptr)
        : QWidget(parent),
          m_borderWidth(5),
          m_phase(0)
    {
        // 设置默认动画间隔为30毫秒
        m_timer.setInterval(30);
        connect(&m_timer, &QTimer::timeout, this, &RGBBorderWidget::updateAnimation);
        m_timer.start();
    }

    ~RGBBorderWidget()
    {
        m_timer.stop();
    }

    // 设置边框宽度
    void setBorderWidth(int width)
    {
        m_borderWidth = width;
        update(); // 重新绘制
    }

    // 设置动画间隔（毫秒）
    void setAnimationInterval(int interval)
    {
        m_timer.setInterval(interval);
    }

  protected:
    void paintEvent(QPaintEvent *event)
    {
        Q_UNUSED(event);

        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        // 定义边框矩形
        QRect rect = this->rect();
        rect.adjust(m_borderWidth / 2, m_borderWidth / 2, -m_borderWidth / 2, -m_borderWidth / 2);

        // 创建一个笔刷
        QPen pen;
        pen.setWidth(m_borderWidth);

        // 生成颜色渐变
        QConicalGradient gradient(rect.center(), 0); // 使用圆锥渐变来实现围绕中心的完整360度旋转
        for (int i = 0; i < 360; i += 10)
        {
            // 计算颜色，使用相位来进行动画
            QColor color;
            color.setHsv((i + m_phase) % 360, 255, 255);
            gradient.setColorAt(static_cast<qreal>(i) / 360.0, color);
        }

        pen.setBrush(QBrush(gradient));
        painter.setPen(pen);

        // 绘制边框
        painter.drawRect(rect);
    }

  private slots:
    void updateAnimation()
    {
        m_phase = (m_phase + 1) % 360; // 保持相位在0-359度之间
        update();                      // 触发重绘
    }

  private:
    int m_borderWidth;
    QTimer m_timer;
    int m_phase; // 用于控制颜色变化的相位
};