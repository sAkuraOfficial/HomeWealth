#pragma once
#include "ui_page_main.h"
#include <QPieLegendMarker>
#include <QVBoxLayout>
#include <QWidget>
#include <QtCharts/QChartView>
#include <QtCharts/QLegendMarker>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>

QT_USE_NAMESPACE

// page_main 类
class page_main : public QWidget
{
    Q_OBJECT

  public:
    page_main(QWidget *parent = nullptr);
    ~page_main();

  private:
    void setupDonutChart(); // 初始化甜甜圈图

    Ui::page_mainClass ui;
};
