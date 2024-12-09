#include "page_category.h"

page_category::page_category(Core *core, user_info user_info, QWidget *parent)
    : QWidget(parent), m_core(core), m_user_info(user_info)
{
    ui.setupUi(this);
    setupTableView();
    m_core->getCategory(m_user_info.family_id);
    connect(m_core, &Core::ReceiveGetCategory, this, &page_category::onGetCategory);
}

page_category::~page_category()
{
}

void page_category::onItemChanged(QStandardItem *item)
{
    int row = item->row();
    int column = item->column();
    int category_id = model->item(row, 0)->text().toInt();
    QString category_name = model->item(row, 1)->text();
    bool is_income = model->item(row, 2)->text() == "收入";
    category_info category = {category_id, category_name, is_income};
    m_core->UpdateCategoryData(category);
}

void page_category::on_pushButton_addRow_clicked()
{
    dialog_inputCategory *dialog = new dialog_inputCategory(m_user_info.family_id, m_core, this);
    dialog->exec();
    // 释放
    dialog->deleteLater();
    // 刷新
    m_core->getCategory(m_user_info.family_id);
}

void page_category::on_pushButton_editRow_clicked()
{
    QModelIndexList selectedItem = ui.tableView->selectionModel()->selectedIndexes();
    if (selectedItem.isEmpty())
    {
        QMessageBox::warning(this, "警告", "请选中一行", QMessageBox::Ok);
        return;
    }
    int row = selectedItem.first().row();
    int category_id = model->item(row, 0)->text().toInt();

    // 提示框
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "确认删除", "确定要删除选中的行吗？", QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::No)
    {
        return;
    }
    else
    {
        // 调用删除函数
        m_core->deleteCategoryData(category_id);
        // 刷新
        m_core->getCategory(m_user_info.family_id);
    }
}

void page_category::setupTableView()
{
    model = new QStandardItemModel(this);
    ui.tableView->setModel(model);
    ui.tableView->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked);
    // 设置表头
    QStringList header = {"category_id", "分类名", "支出类型"};
    model->setHorizontalHeaderLabels(header);
    // 设置表头自适应
    ui.tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    // 设置表头居中
    ui.tableView->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
    // 设置字体微软雅黑
    ui.tableView->horizontalHeader()->setFont(QFont("Microsoft YaHei", 14));
    ui.tableView->setFont(QFont("Microsoft YaHei", 14));
    delegate_radio *delegate = new delegate_radio(this);
    ui.tableView->setItemDelegateForColumn(2, delegate);
    connect(model, &QStandardItemModel::itemChanged, this, &page_category::onItemChanged);
}

void page_category::onGetCategory(QVector<category_info> categories)
{
    disconnect(model, &QStandardItemModel::itemChanged, this, &page_category::onItemChanged);
    model->removeRows(0, model->rowCount());
    for (int i = 0; i < categories.size(); i++)
    {
        category_info category = categories[i];
        QStandardItem *item0 = new QStandardItem(QString::number(category.category_id));
        QStandardItem *item1 = new QStandardItem(category.category_name);
        QStandardItem *item2 = new QStandardItem(category.is_income ? "收入" : "支出");
        model->setItem(i, 0, item0);
        model->setItem(i, 1, item1);
        model->setItem(i, 2, item2);
    }
    // 设置表格居中
    for (int i = 0; i < model->rowCount(); i++)
    {
        model->item(i, 2)->setTextAlignment(Qt::AlignCenter);
        model->item(i, 0)->setEditable(false); // 设置不可编辑
    }
    connect(model, &QStandardItemModel::itemChanged, this, &page_category::onItemChanged);
}
