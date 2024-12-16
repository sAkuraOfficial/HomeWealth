#include "page_family.h"
#include "dialog_insertUser/dialog_insertUser.h"
#include <QLineEdit>
page_family::page_family(Core *core, user_info user, QWidget *parent)
    : QWidget(parent), m_core(core), m_user_info(user)
{
    ui.setupUi(this);
    // 设置图片
    ui.label_profile->setFixedSize(75, 75);
    ui.label_profile->setPixmap(QPixmap(":/Client/profile"));
    ui.label_show_login_name->setText(m_user_info.username);
    if (m_user_info.family_id == -1)
    {
        ui.label_show_family_id->setText("您还未加入家庭！");
    }
    else
    {
        ui.label_show_family_id->setText("所属家庭号：" + QString::number(m_user_info.family_id));
    }

    ui.label_user_type->setText(m_user_info.isAdmin ? "管理员" : "普通用户");
    // 设置图片填充到label中
    ui.label_profile->setScaledContents(true);
    // 设置label的大小
    ui.rgbWidget->layout()->setContentsMargins(3, 3, 3, 3);
    ui.rgbWidget->setBorderWidth(3);
    ui.rgbWidget->setAnimationInterval(1);
    setupTable();
    connect(m_core, &Core::ReceiveGetFamilyUserList, this, &page_family::onReceiveGetFamilyUserList);
    connect(m_core, &Core::ReceiveDeleteUserFromFamily, this, &page_family::onReceiveDeleteUserFromFamily);
}

page_family::~page_family()
{
}

void page_family::on_pushButton_addRow_clicked()
{
    if (m_user_info.isAdmin)
    {
        dialog_insertUser dialog(m_core, m_user_info, this);
        dialog.exec();
    }
    else
    {
        QMessageBox::warning(this, "错误", "您还不是管理员，权限不足");
    }
}

void page_family::on_pushButton_deleteRow_clicked()
{
    if (m_user_info.isAdmin)
    {
        QModelIndex index = ui.tableView->currentIndex();
        if (index.isValid())
        {
            int row = index.row();
            int user_id = model->item(row, 0)->text().toInt();
            m_core->deleteUserFromFamily(m_user_info.family_id, user_id);
        }
        else
        {
            QMessageBox::warning(this, "错误", "请选中一行");
        }
    }
    else
    {
        QMessageBox::warning(this, "错误", "您还不是管理员，权限不足");
    }
}

void page_family::on_pushButton_refresh_clicked()
{
}

void page_family::onReceiveGetFamilyUserList(QVector<user_info> family_user_list)
{
    m_family_user_list = family_user_list;
    model->removeRows(0, model->rowCount());
    for (int i = 0; i < family_user_list.size(); i++)
    {
        model->setItem(i, 0, new QStandardItem(QString::number(family_user_list[i].user_id)));
        model->setItem(i, 1, new QStandardItem(family_user_list[i].username));
        model->setItem(i, 2, new QStandardItem(family_user_list[i].isAdmin ? "是" : "否"));
    }
    ui.tableView->setModel(model);
    ui.tableView->show();
}

void page_family::onReceiveDeleteUserFromFamily(bool result, QString error_msg)
{
    if (result)
    {
        QMessageBox::information(this, "成功", "删除成功");
        m_core->getFamilyUserList(m_user_info.family_id);
    }
    else
    {
        QMessageBox::warning(this, "错误", error_msg);
    }
}

void page_family::setupTable()
{
    // 家庭成员表格，userid,loginname,is_admin
    model = new QStandardItemModel();
    model->setColumnCount(3);
    model->setHeaderData(0, Qt::Horizontal, "用户ID");
    model->setHeaderData(1, Qt::Horizontal, "登录名");
    model->setHeaderData(2, Qt::Horizontal, "是否管理员");

    ui.tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui.tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui.tableView->setSelectionBehavior(QAbstractItemView::SelectRows);  // 设置选中一行
    ui.tableView->setSelectionMode(QAbstractItemView::SingleSelection); // 设置只能选中一行
    ui.tableView->setModel(model);
    ui.tableView->setSortingEnabled(true);
    ui.tableView->sortByColumn(0, Qt::AscendingOrder);
    ui.tableView->verticalHeader()->hide();
    ui.tableView->setItemDelegateForColumn(0, new Delegate_tableItemAlignCenter());
    ui.tableView->setItemDelegateForColumn(1, new Delegate_tableItemAlignCenter());
    ui.tableView->setItemDelegateForColumn(2, new Delegate_tableItemAlignCenter());
}
