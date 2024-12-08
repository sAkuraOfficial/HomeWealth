#include "page_edit.h"
#include "delegate_combox/delegate_combox.h"
#include <QInputDialog>
#include <logger/logger.h>
#include <qjsonarray.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qpushbutton.h>
page_edit::page_edit(Core *core, user_info user_info, QWidget *parent)
    : QWidget(parent),
      m_core(core), m_user_info(user_info)
{
    ui.setupUi(this);
    setupTableView();

    // 设置控件状态
    ui.DateEdit_searchTime_From->setDate(QDate::currentDate().addDays(-30));
    ui.DateEdit_searchTime_To->setDate(QDate::currentDate());

    // 连接 core 的 dataReceived 信号到 populateTable 槽
    connect(m_core, &Core::dataReceived, this, &page_edit::populateTable);
    connect(ui.pushButton_addRow, &QPushButton::clicked, this, &page_edit::onAddRowClicked);
    connect(m_core, &Core::ReceiveGetCategory, this, &page_edit::onGetCategory);
    connect(m_core, &Core::ReceiveGetFamilyUserList, this, &page_edit::onReceiveGetFamilyUserList);
    connect(ui.radioButton_selectType_all, &QRadioButton::clicked, this, &page_edit::onRadioButtonClicked);
    connect(ui.radioButton_selectType_incmome, &QRadioButton::clicked, this, &page_edit::onRadioButtonClicked);
    connect(ui.radioButton_selectType_expense, &QRadioButton::clicked, this, &page_edit::onRadioButtonClicked);
    // 请求数据
    m_core->getFamilyUserList(m_user_info.family_id); // 请求家庭成员列表
    m_core->getCategory(m_user_info.family_id);       // 请求分类数据
    m_core->getDataRequest(m_user_info.user_id);      // 请求明细数据
}

page_edit::~page_edit()
{
}

void page_edit::setupTableView()
{
    model = new QStandardItemModel(this);
    ui.tableView->setModel(model);
    ui.tableView->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked);

    // 设置表头
    QStringList headers = {"明细ID", "家庭ID", "用户ID", "分类ID", "用户名", "分类名", "明细内容", "金额", "修改日期"};
    model->setHorizontalHeaderLabels(headers);
    ui.tableView->setColumnHidden(0, true); // 隐藏 'detail_id' 列
    ui.tableView->setColumnHidden(1, true); // 隐藏 'family_id' 列
    ui.tableView->setColumnHidden(2, true); // 隐藏 'user_id' 列
    ui.tableView->setColumnHidden(3, true); // 隐藏 'category_id' 列
    // 删除行标
    // ui.tableView->verticalHeader()->hide();
    // 自动列宽
    ui.tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    // 设置文字居中
    ui.tableView->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
    // 设置字体微软雅黑
    ui.tableView->horizontalHeader()->setFont(QFont("Microsoft YaHei", 14));
    ui.tableView->setFont(QFont("Microsoft YaHei", 14));
    //// 创建并设置 ComboBoxDelegate
    // delegate_combox *comboBoxDelegate = new delegate_combox(this);
    // ui.tableView->setItemDelegateForColumn(4, comboBoxDelegate); // 为 'category_name' 列设置代理
    //  连接 itemChanged 信号
    connect(model, &QStandardItemModel::itemChanged, this, &page_edit::onItemChanged);
}

void page_edit::populateTable(QJsonArray dataArray)
{
    disconnect(model, &QStandardItemModel::itemChanged, this, &page_edit::onItemChanged);
    model->removeRows(0, model->rowCount()); // 清除现有数据
    for (int i = 0; i < dataArray.size(); ++i)
    {
        QJsonObject record = dataArray[i].toObject();
        QList<QStandardItem *> rowItems;
        rowItems << new QStandardItem(QString::number(record["detail_id"].toInt()));
        rowItems << new QStandardItem(QString::number(record["family_id"].toInt()));
        rowItems << new QStandardItem(QString::number(record["user_id"].toInt()));
        rowItems << new QStandardItem(QString::number(record["category_id"].toInt()));
        rowItems << new QStandardItem(record["login_name"].toString());
        rowItems << new QStandardItem(record["category_name"].toString());
        rowItems << new QStandardItem(record["description"].toString());
        rowItems << new QStandardItem(QString::number(record["amount"].toDouble()));
        rowItems << new QStandardItem(record["transaction_date"].toString());
        model->appendRow(rowItems);
    }

    // 根据需要设置哪些列可编辑
    for (int row = 0; row < model->rowCount(); ++row)
    {
        // 若id与当前客户端id不一样，设置整行不可编辑
        int user_id = model->item(row, 2)->text().toInt();
        if (user_id != m_user_info.user_id)
        {
            for (int column = 0; column < model->columnCount(); ++column)
            {
                QStandardItem *item = model->item(row, column);
                item->setEditable(false);
            }
            continue;
        }
        for (int column = 0; column < model->columnCount(); ++column)
        {
            QStandardItem *item = model->item(row, column);
            if (column == 1 || column == 2 || column == 3 || column == 4) // 禁止编辑的列
            {
                item->setEditable(false);
            }
            else
            {
                item->setEditable(true);
            }
        }
    }

    connect(model, &QStandardItemModel::itemChanged, this, &page_edit::onItemChanged); // 恢复信号连接
}

void page_edit::onItemChanged(QStandardItem *item)
{
    int row = item->row();
    int column = item->column();
    QVariant newValue = item->text();

    // 获取 'detail_id' 以标识数据库中的记录
    int detail_id = model->item(row, 0)->text().toInt();

    // 将列索引映射到数据库字段名
    QString column_name;
    switch (column)
    {
    case 1:
        column_name = "family_id";
        break;
    case 2:
        column_name = "user_id";
        break;
    case 3:
        column_name = "category_id";
        break;
    case 4:
        column_name = "category_name";
        break;
    case 5:
        column_name = "login_name";
        break;
    case 6:
        column_name = "description";
        break;
    case 7:
        column_name = "amount";
        break;
    case 8:
        column_name = "transaction_date";
        break;
    default:
        return; // 不可编辑的字段或无效的列
    }
    if (column_name == "login_name")
    {
        return;
    }
    if (column_name == "category_name")
    {
        // 做特定处理
        // Logger::getInstance().log("category_name changed");
        // Logger::getInstance().log("detail_id: " + QString::number(detail_id));
        // Logger::getInstance().log("newValue: " + newValue.toString());
        // 通过 category_name 获取 category_id
        int category_id = -1;
        for (auto category : m_category_info)
        {
            if (category.category_name == newValue.toString())
            {
                category_id = category.category_id;
                break;
            }
        }
        if (category_id == -1)
        {
            Logger::getInstance().log("未找到对应的 category_id");
            return;
        }
        m_core->UpdateDetialData(detail_id, "category_id", category_id);
        return;
    }
    m_core->UpdateDetialData(detail_id, column_name, newValue);
}

void page_edit::onAddRowClicked()
{
    dialog_input *dialog = new dialog_input(m_user_info, m_core, m_category_info, this);
    dialog->exec();
    // 释放
    dialog->deleteLater();
    // 刷新
    // m_core->getDataRequest(m_user_info.user_id);
    on_toolButton_search_clicked();
}

void page_edit::onInsertDataResponse(QJsonObject json)
{
}

void page_edit::onGetCategory(QVector<category_info> categories)
{
    m_category_info = categories;
    delegate_combox *comboBoxDelegate = new delegate_combox(this);
    QVector<QString> categoryNames;
    for (auto category : categories)
    {
        categoryNames.push_back(category.category_name);
    }
    comboBoxDelegate->setOptions(categoryNames);
    ui.tableView->setItemDelegateForColumn(5, comboBoxDelegate); // 为 'category_name' 列设置代理
}

void page_edit::onReceiveGetFamilyUserList(QVector<user_info> family_user_list)
{
    m_family_user_list = family_user_list;
}

void page_edit::on_pushButton_deleteRow_clicked()
{
    QModelIndexList selectedItem = ui.tableView->selectionModel()->selectedIndexes();
    if (selectedItem.isEmpty())
    {
        return;
    }
    int row = selectedItem.first().row();
    int detail_id = model->item(row, 0)->text().toInt();

    // 提示框
    QMessageBox::StandardButton reply;
    // 判断所在行id是否为当前用户id
    if (m_user_info.user_id != model->item(row, 2)->text().toInt())
    {
        QMessageBox::information(this, "提示", "只能删除自己的记录！");
        return;
    }
    reply = QMessageBox::question(this, "确认删除", "确定要删除选中的行吗？", QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::No)
    {
        return;
    }
    else
    {
        // 调用删除函数
        m_core->deleteDetialData(detail_id);
        // 刷新
        /* m_core->getDataRequest(m_user_info.user_id);*/
        on_toolButton_search_clicked();
    }
}

void page_edit::on_pushButton_refresh_clicked()
{
    /*m_core->getDataRequest(m_user_info.user_id);*/
    on_toolButton_search_clicked();
}

void page_edit::on_checkBox_searchUser_selectMe_clicked()
{
    if (ui.checkBox_searchUser_selectMe->isChecked())
    {
        ui.pushButton_searchUser_selectFromFamily->setEnabled(false);
        on_toolButton_search_clicked();
    }
    else
    {
        ui.pushButton_searchUser_selectFromFamily->setEnabled(true);
    }
}

void page_edit::on_checkBox_searchTime_all_clicked()
{
    if (ui.checkBox_searchTime_all->isChecked())
    {
        ui.DateEdit_searchTime_From->setEnabled(false);
        ui.DateEdit_searchTime_To->setEnabled(false);
        ui.label_to->setEnabled(false);
        on_toolButton_search_clicked();
    }
    else
    {
        ui.DateEdit_searchTime_From->setEnabled(true);
        ui.DateEdit_searchTime_To->setEnabled(true);
        ui.label_to->setEnabled(true);
    }
}

void page_edit::on_pushButton_searchUser_selectFromFamily_clicked()
{
    if (m_family_user_list.isEmpty())
    {
        QMessageBox::information(this, "提示", "家庭成员列表为空！");
        return;
    }

    // 创建并显示多选对话框
    dialog_multiSelect_for_familyUser dialog(m_family_user_list, m_selected_family_user, this);
    if (dialog.exec() == QDialog::Accepted)
    {
        m_selected_family_user = dialog.getSelectedUsers();

        // 显示选中的成员
        QStringList selectedNames;
        for (const auto &user : m_selected_family_user)
        {
            selectedNames << user.username;
        }
        QString selectedText = selectedNames.join(", ");
        // QMessageBox::information(this, "已选择的成员", selectedText);
        on_toolButton_search_clicked();
    }
}

void page_edit::on_pushButton_searchCategory_selectFromCategoryList_clicked()
{
    // 创建并显示多选对话框

}

void page_edit::on_toolButton_search_clicked()
{
    QString keyword = ui.lineEdit_search_keyword->text();
    QVector<user_info> search_selectedUsers;
    searchType search_type;
    bool search_time_all = ui.checkBox_searchTime_all->isChecked();
    QDateTime search_time_from;
    QDateTime search_time_to;
    // 判断时间
    if (!ui.checkBox_searchTime_all->isChecked())
    {
        search_time_from = ui.DateEdit_searchTime_From->dateTime();
        search_time_to = ui.DateEdit_searchTime_To->dateTime();
    }
    // 判断收入支出类型
    if (ui.radioButton_selectType_all->isChecked())
    {
        search_type = searchType::all; // 搜索全部
    }
    else if (ui.radioButton_selectType_incmome->isChecked())
    {
        search_type = searchType::income; // 只搜索收入
    }
    else
    {
        search_type = searchType::expense; // 只搜索支出
    }
    // 判断输入框
    // if (keyword.isEmpty())
    //{
    //    QMessageBox::information(this, "提示", "请输入关键字！");
    //    return;
    //}
    // 判断是否只搜索自己
    if (ui.checkBox_searchUser_selectMe->isChecked())
    {
        search_selectedUsers.push_back(m_user_info);
    }
    else
    {
        search_selectedUsers = m_selected_family_user;
    }
    // 发送请求
    m_core->getDataRequestEx(m_user_info.user_id, keyword, search_selectedUsers, search_time_all, search_type, search_time_all, search_time_from, search_time_to);
}

void page_edit::onRadioButtonClicked()
{
    on_toolButton_search_clicked();
}

void page_edit::on_lineEdit_search_keyword_textChanged()
{
    on_toolButton_search_clicked();
}
