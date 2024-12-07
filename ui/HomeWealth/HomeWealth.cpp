#include "HomeWealth.h"
#include "core/core.h"
HomeWealth::HomeWealth(Core *core, QWidget *parent)
    : QMainWindow(parent), m_core(core)
{
    ui.setupUi(this);
    // 设置窗口不可调整大小
    this->setFixedSize(this->width(), this->height());
    // 隐藏最大化按钮
    this->setWindowFlags(Qt::Window | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);

    connect(ui.listWidget_page, &QListWidget::itemClicked, this, &HomeWealth::onListItemClicked);
    connect(m_core, &Core::signal_disconnect, this, &HomeWealth::onDisconnected); // 断开连接
}

HomeWealth::~HomeWealth()
{
}

void HomeWealth::addPage(PageType page)
{
    QWidget *widget = nullptr;
    QString alias; // 用于存储页面别名

    // 根据 PageType 创建对应页面
    switch (page)
    {
    case PageType::main:
        widget = new page_main(this); // 创建主页面
        alias = "Main Page";          // 设置页面别名
        break;
    //case PageType::input:
    //    widget = new page_input(this);
    //    alias = "Input Page"; // 设置页面别名
    //    break;
    case PageType::edit:
        widget = new page_edit(m_core,m_user_info, this); // 创建 page_edit 页面
        alias = "Edit Page";
        break;
    default:
        return; // 未知页面类型
    }

    // 添加页面到 QStackedWidget
    if (widget)
    {
        ui.stackedWidget->addWidget(widget);
        m_pages[page] = std::make_pair(widget, alias); // 保存页面和别名到映射表

        // 往 listWidget_page 中添加项，使用别名
        ui.listWidget_page->addItem(new QListWidgetItem(alias));
    }
}

void HomeWealth::setPage(PageType page)
{
    if (m_pages.find(page) != m_pages.end())
    {
        ui.stackedWidget->setCurrentWidget(m_pages[page].first);
    }
}

void HomeWealth::onLoginSuccess(QString username, int user_id, bool is_admin, int family_id)
{
    m_user_info.username = username;
    m_user_info.user_id = user_id;
    m_user_info.isAdmin = is_admin;
    m_user_info.family_id = family_id;
    m_core->setClientUserInfo(&m_user_info); // 设置用户信息,方便内核使用
    addPage(PageType::main);  // 添加主页面
    addPage(PageType::edit);  // 编辑页面
    setPage(PageType::main);  // 设置主页面
    this->show();
}

void HomeWealth::onListItemClicked(QListWidgetItem *item)
{
    if (!item)
        return;

    QString clickedAlias = item->text(); // 获取点击的别名

    // 遍历页面映射表，找到对应的 PageType
    for (auto it = m_pages.begin(); it != m_pages.end(); ++it)
    {
        if (it->second.second == clickedAlias) // 匹配别名
        {
            setPage(it->first); // 切换到对应的页面
            return;
        }
    }
}

void HomeWealth::onDisconnected()
{
    // 如果主窗口已经显示（表示已完成登录）
    if (this->isVisible())
    {
        int ret = QMessageBox::warning(this, tr("连接断开"), tr("与服务器的连接已断开。是否重新连接？"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        if (ret == QMessageBox::Yes)
        {
            // 用户选择重连，这里可以使用之前连接的地址和超时参数重新连接
            // 假设我们在登录成功后保存在core中，也可以在登录时保存一下连接信息
            // 例如核心中 m_wsUrl 和 m_timeoutMs 已经记下了，如果没有记下需要你在runClient时记录
            m_core->runClient("ws://localhost:2222", 5000);
        }
        else
        {
            this->hide();
            emit backToLogin(); // 返回到登录窗口
        }
    }
    else
    {
        // 主窗口未显示的情况(实际上不会在登录前显示主窗口)
        // 不做额外处理，登录窗口自身有重连逻辑
    }
}