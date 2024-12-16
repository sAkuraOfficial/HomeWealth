#include "page_main.h"
#include <QLineSeries>
#include <QValueAxis>
#include<QGraphicsLayout>
page_main::page_main(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    //setupDonutChart();
}

page_main::~page_main() = default;
//
//void page_main::setupDonutChart()
//{
//    QLineSeries *series = new QLineSeries();
//    series->append(0, 1);
//    series->append(1, 3);
//    series->append(2, 2);
//    series->append(3, 4);
//    series->append(4, 3);
//
//    // 创建图表
//    QChart *chart = new QChart();
//    chart->addSeries(series);
//    //chart->setTitle("最近几个月的收入情况");
//    chart->setTitleFont(QFont("Microsoft YaHei", 12, QFont::Bold));
//    chart->setMargins(QMargins(0, 0, 0, 0)); // 设置图表边距
//    chart->setContentsMargins(0, 0, 0, 0);   // 设置图表内容边距
//    chart->legend()->hide(); // 隐藏图例
//    chart->layout()->setContentsMargins(0, 0, 0, 0);
//    //添加动画
//    chart->setAnimationOptions(QChart::AllAnimations);
//
//
//    // 配置 X 轴
//    QValueAxis *axisX = new QValueAxis();
//    //axisX->setTitleText("X 轴");
//    axisX->setLabelFormat("%d");
//    axisX->setRange(0, 4);
//    chart->addAxis(axisX, Qt::AlignBottom);
//    series->attachAxis(axisX);
//
//    // 配置 Y 轴
//    QValueAxis *axisY = new QValueAxis();
//    //axisY->setTitleText("Y 轴");
//    axisY->setLabelFormat("%d");
//    axisY->setRange(0, 4);
//    chart->addAxis(axisY, Qt::AlignLeft);
//    series->attachAxis(axisY);
//
//    // 创建图表视图
//    QChartView *chartView = new QChartView(chart);
//    chartView->setRenderHint(QPainter::Antialiasing);
//    chartView->setContentsMargins(0, 0, 0, 0); // 设置图表视图边距
//    chartView->setStyleSheet("border: 0px; padding: 0px;");
//
//      // 创建自定义标题 QLabel
//    QLabel *titleLabel = new QLabel("最近几个月的收入情况");
//    titleLabel->setAlignment(Qt::AlignCenter);
//    titleLabel->setFont(QFont("Microsoft YaHei", 12, QFont::Bold));
//
//    // 设置图表布局到 widget_show_chart
//    QVBoxLayout *layout = new QVBoxLayout();
//    
//    ui.widget_show_chart->setLayout(layout);
//    ui.widget_show_chart->setContentsMargins(5, 5, 5, 5); // 设置 widget_show_chart 边距
//    layout->setContentsMargins(0,0,0,0); // 移除布局边距
//    layout->setSpacing(0);                  // 移除控件间距
//
//    layout->addWidget(chartView);
//    layout->addWidget(titleLabel);
//}
