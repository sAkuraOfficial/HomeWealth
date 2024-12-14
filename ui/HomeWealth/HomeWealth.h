#pragma once

#include "ui/HomeWealth/page/page_edit/page_edit.h"
#include "ui/HomeWealth/page/page_main/page_main.h"
#include "ui/HomeWealth/page/page_category/page_category.h"
#include "ui_HomeWealth.h"
#include <QtWidgets/QMainWindow>
#include <core/core.h>
#include <AnimatedStackedWidget.hpp>
class HomeWealth : public QMainWindow
{
    Q_OBJECT

  public:
    HomeWealth(Core *core, QWidget *parent = nullptr);
    ~HomeWealth();
    enum PageType
    {
        main,
        edit,
        category
    };
    void addPage(PageType page);
    void setPage(PageType page);

  private:
    Ui::HomeWealth ui;
    Core *m_core;
    std::map<PageType, std::pair<QWidget *, QString>> m_pages; // 添加页面别名
    user_info m_user_info;
    family_info m_family_info;
  public slots:
    void onLoginSuccess(QString username, int user_id, bool is_admin, int family_id);
  private slots:
    void onListItemClicked(QListWidgetItem *item);
    void onDisconnected();
  signals:
    void backToLogin();
};
