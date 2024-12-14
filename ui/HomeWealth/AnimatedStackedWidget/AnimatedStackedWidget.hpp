#pragma once
#include <QDebug>
#include <QEasingCurve>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include <QStackedWidget>
#include <QWidget>

class AnimatedStackedWidget : public QStackedWidget
{
    Q_OBJECT

  public:
    explicit AnimatedStackedWidget(QWidget *parent = nullptr)
        : QStackedWidget(parent), m_duration(500), m_isAnimating(false)
    {
    }

    // 设置动画持续时间
    void setAnimationDuration(int duration)
    {
        m_duration = duration;
    }

    int animationDuration() const
    {
        return m_duration;
    }

    // 带动画的页面切换
    void slideToIndex(int index)
    {
        if (index < 0 || index >= count() || index == currentIndex() || m_isAnimating)
        {
            return;
        }

        QWidget *currentWidget = this->currentWidget();
        QWidget *nextWidget = widget(index);

        int offset = width();
        if (index < currentIndex())
        {
            offset = -offset; // 左移
        }

        nextWidget->setGeometry(offset, 0, width(), height());
        nextWidget->show();

        // 当前页面动画
        QPropertyAnimation *currentAnimation = new QPropertyAnimation(currentWidget, "geometry");
        currentAnimation->setDuration(m_duration*0.3);
        currentAnimation->setEasingCurve(QEasingCurve::InOutCubic);
        currentAnimation->setStartValue(currentWidget->geometry());
        currentAnimation->setEndValue(QRect(-offset, 0, width(), height()));

        // 下一页面的动画组
        QSequentialAnimationGroup *nextAnimationGroup = new QSequentialAnimationGroup(this);

        // 第一阶段：滑动到目标位置的前面一点
        QPropertyAnimation *slidePastTarget = new QPropertyAnimation(nextWidget, "geometry");
        slidePastTarget->setDuration(m_duration * 0.7);
        slidePastTarget->setEasingCurve(QEasingCurve::OutCubic);
        slidePastTarget->setStartValue(nextWidget->geometry());
        slidePastTarget->setEndValue(QRect(-offset * 0.1, 0, width(), height()));

        // 第二阶段：回弹到目标位置
        QPropertyAnimation *slideToTarget = new QPropertyAnimation(nextWidget, "geometry");
        slideToTarget->setDuration(m_duration * 0.3);
        slideToTarget->setEasingCurve(QEasingCurve::InOutBounce);
        slideToTarget->setStartValue(QRect(-offset * 0.1, 0, width(), height()));
        slideToTarget->setEndValue(QRect(0, 0, width(), height()));

        nextAnimationGroup->addAnimation(slidePastTarget);
        nextAnimationGroup->addAnimation(slideToTarget);

        // 动画完成后切换页面
        connect(nextAnimationGroup, &QSequentialAnimationGroup::finished, this, [=]() {
            QStackedWidget::setCurrentIndex(index);
            currentWidget->hide();
            m_isAnimating = false;

            // 清理动画对象
            currentAnimation->deleteLater();
            nextAnimationGroup->deleteLater();
        });

        m_isAnimating = true;
        currentAnimation->start();
        nextAnimationGroup->start();
    }

    // 自定义页面切换动画
    void setCurrentIndex(int index)
    {
        if (m_isAnimating)
        {
            return;
        }
        slideToIndex(index);
    }

  private:
    int m_duration;     // 动画持续时间（毫秒）
    bool m_isAnimating; // 标记动画状态
};