#include "page_category.h"

page_category::page_category(Core *core, user_info user_info, QWidget *parent)
    : QWidget(parent), m_core(core), m_user_info(user_info)
{
    ui.setupUi(this);
    ui.DateEdit_searchTimeA_To->setDate(QDate::currentDate());
    ui.DateEdit_searchTimeB_To->setDate(QDate::currentDate());
    setupTableView();
    m_core->getCategory(m_user_info.family_id);
    connect(m_core, &Core::ReceiveGetCategory, this, &page_category::onGetCategory);
    connect(m_core, &Core::ReceiveGetFamilyUserList, this, &page_category::onGetFamilyUserList);
    connect(m_core, &Core::ReceiveGetCategorySummary, this, &page_category::onReceiveGetCategorySummary);
    connect(this, &page_category::update_UI_under_summary, this, &page_category::onUpdate_UI_under_summary);
    connect(m_core, &Core::ReceiveUpdateCategoryData, this, [=] {
        m_core->getCategory(m_user_info.family_id); // 获取最新数据
    });
    connect(ui.radioButton_selectType_all, &QRadioButton::clicked, this, [=] {
        ui.radioButton_selectType_all->setChecked(true); // 因为按钮被点击后不是立刻选中，所以需要手动设置
        ui.radioButton_selectType_expense->setChecked(false);
        ui.radioButton_selectType_incmome->setChecked(false);
        m_core->getCategory(m_user_info.family_id);
    });
    connect(ui.radioButton_selectType_incmome, &QRadioButton::clicked, this, [=] {
        ui.radioButton_selectType_incmome->setChecked(true);
        ui.radioButton_selectType_all->setChecked(false);
        ui.radioButton_selectType_expense->setChecked(false);
        m_core->getCategory(m_user_info.family_id);
    });
    connect(ui.radioButton_selectType_expense, &QRadioButton::clicked, this, [=] {
        m_core->getCategory(m_user_info.family_id);
        ui.radioButton_selectType_expense->setChecked(true);
        ui.radioButton_selectType_all->setChecked(false);
        ui.radioButton_selectType_incmome->setChecked(false);
    });
    connect(ui.radioButton_selectType_summary_all, &QRadioButton::clicked, this, [=] {
        on_pushButton_summary_get_data_clicked();
    });
    connect(ui.radioButton_selectType_summary_expense, &QRadioButton::clicked, this, [=] {
        on_pushButton_summary_get_data_clicked();
    });
    connect(ui.radioButton_selectType_summary_incmome, &QRadioButton::clicked, this, [=] {
        on_pushButton_summary_get_data_clicked();
    });
    connect(ui.checkBox_summary_all_family, &QCheckBox::clicked, this, [=] {
        on_pushButton_summary_get_data_clicked();
    });
    connect(ui.checkBox_summary_all_category, &QCheckBox::clicked, this, [=] {
        on_pushButton_summary_get_data_clicked();
    });
}

page_category::~page_category()
{
}

void page_category::onGetFamilyUserList(QVector<user_info> family_user_list)
{
    m_family_user_list = family_user_list;
}

void page_category::onItemChanged(QStandardItem *item)
{
    int row = item->row();
    int column = item->column();
    int category_id = model->item(row, 0)->text().toInt();
    QString category_name = model->item(row, 1)->text();
    bool is_income = model->item(row, 2)->text() == "收入";
    category_summary category = {category_id, category_name, is_income};
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

void page_category::on_pushButton_switchPage_to_edit_clicked()
{
    // 切换到第0页
    // ui.stackedWidget->setCurrentIndex(0);
    ui.stackedWidget->slideToIndex(0);
}

void page_category::on_pushButton_switchPage_to_summary_clicked()
{
    // 切换到第1页
    // ui.stackedWidget->setCurrentIndex(1);
    ui.stackedWidget->slideToIndex(1);
}

void page_category::on_pushButton_summary_get_data_clicked()
{
    QDateTime time_A_from = ui.DateEdit_searchTimeA_From->dateTime();
    QDateTime time_A_to = ui.DateEdit_searchTimeA_To->dateTime();
    QDateTime time_B_from = ui.DateEdit_searchTimeB_From->dateTime();
    QDateTime time_B_to = ui.DateEdit_searchTimeB_To->dateTime();
    // 判断时间范围是否合法
    if (time_A_from > time_A_to || time_B_from > time_B_to)
    {
        QMessageBox::warning(this, "警告", "时间范围不合法", QMessageBox::Ok);
        return;
    }
    // 获取选中的分类
    if (ui.checkBox_summary_all_category->isChecked())
        m_category_info_selected = m_category_info;
    QVector<int> category_ids;
    for (const auto &category : m_category_info_selected)
    {
        category_ids << category.category_id;
    }
    // 获取选中的家庭成员
    if (ui.checkBox_summary_all_family->isChecked())
        m_family_user_list_selected = m_family_user_list;
    QVector<int> user_ids;
    for (const auto &user : m_family_user_list_selected)
    {
        user_ids << user.user_id;
    }
    // 获取查询类型
    searchType type;
    if (ui.radioButton_selectType_summary_all->isChecked())
        type = searchType::all;
    else if (ui.radioButton_selectType_summary_incmome->isChecked())
        type = searchType::income;
    else if (ui.radioButton_selectType_summary_expense->isChecked())
        type = searchType::expense;

    m_core->getCategorySummary(
        m_user_info.family_id,
        user_ids,
        category_ids,
        type,
        false, // 多余了，以后再改
        time_A_from,
        time_A_to,
        time_B_from,
        time_B_to
    );
}

void page_category::on_pushButton_summary_selectFromFamily_clicked()
{
    if (m_family_user_list.isEmpty())
    {
        QMessageBox::information(this, "提示", "家庭成员列表为空！");
        return;
    }

    // 创建并显示多选对话框
    dialog_multiSelect_for_familyUser dialog(m_family_user_list, m_family_user_list_selected, this);
    if (dialog.exec() == QDialog::Accepted)
    {
        m_family_user_list_selected = dialog.getSelectedUsers();

        // 显示选中的成员
        QStringList selectedNames;
        for (const auto &user : m_family_user_list_selected)
        {
            selectedNames << user.username;
        }
        QString selectedText = selectedNames.join(", ");
        // QMessageBox::information(this, "已选择的成员", selectedText);
        // on_toolButton_search_clicked();
        on_pushButton_summary_get_data_clicked(); // 立即筛选数据
    }
}

void page_category::on_pushButton_summary_selectFromCategoryList_clicked()
{
    // 创建并显示多选对话框
    if (m_category_info.isEmpty())
    {
        QMessageBox::information(this, "提示", "分类列表为空！");
        return;
    }
    dialog_multiSelect_for_category dialog(m_category_info, m_category_info_selected, this);
    if (dialog.exec() == QDialog::Accepted)
    {
        m_category_info_selected = dialog.getSelectedCategory();

        // 显示选中的分类
        QStringList selectedNames;
        for (const auto &category : m_category_info_selected)
        {
            selectedNames << category.category_name;
        }
        QString selectedText = selectedNames.join(", ");
        // QMessageBox::information(this, "已选择的分类", selectedText);
        on_pushButton_summary_get_data_clicked(); // 立即筛选数据
    }
}

void page_category::onReceiveGetCategorySummary(QVector<QVector<category_summary>> category_summary_result)
{
    QVector<category_summary> category_summary_A = category_summary_result[0];
    QVector<category_summary> category_summary_B = category_summary_result[1];
    // 清空model_A和model_B
    model_A->removeRows(0, model_A->rowCount());
    model_B->removeRows(0, model_B->rowCount());
    // 添加数据
    for (int i = 0; i < category_summary_A.size(); i++)
    {
        category_summary summary = category_summary_A[i];
        QStandardItem *item0 = new QStandardItem(QString::number(summary.category_id));
        QStandardItem *item1 = new QStandardItem(summary.category_name);
        QStandardItem *item2 = new QStandardItem(summary.is_income ? "收入" : "支出");
        QStandardItem *item3 = new QStandardItem(QString::number(summary.amount));
        model_A->setItem(i, 0, item0);
        model_A->setItem(i, 1, item1);
        model_A->setItem(i, 2, item2);
        model_A->setItem(i, 3, item3);
        if (summary.is_income)
        {
            m_summary_A_edit_income += summary.amount; 
        }
        else
        {
            m_summary_A_edit_expense += summary.amount;
        }
        m_summary_A_edit_all_income = m_summary_A_edit_income - m_summary_A_edit_expense;
    }

    for (int i = 0; i < category_summary_B.size(); i++)
    {
        category_summary summary = category_summary_B[i];
        QStandardItem *item0 = new QStandardItem(QString::number(summary.category_id));
        QStandardItem *item1 = new QStandardItem(summary.category_name);
        QStandardItem *item2 = new QStandardItem(summary.is_income ? "收入" : "支出");
        QStandardItem *item3 = new QStandardItem(QString::number(summary.amount));
        model_B->setItem(i, 0, item0);
        model_B->setItem(i, 1, item1);
        model_B->setItem(i, 2, item2);
        model_B->setItem(i, 3, item3);
        if (summary.is_income)
        {
            m_summary_B_edit_income += summary.amount;
        }
        else
        {
            m_summary_B_edit_expense += summary.amount;
        }
        m_summary_B_edit_all_income = m_summary_B_edit_income - m_summary_B_edit_expense;
    }
    emit update_UI_under_summary();//更新下面的数据


    // 设置表格居中
    for (int i = 0; i < model_A->rowCount(); i++)
    {
        model_A->item(i, 2)->setTextAlignment(Qt::AlignCenter);
        model_A->item(i, 0)->setEditable(false); // 设置不可编辑
    }
    for (int i = 0; i < model_B->rowCount(); i++)
    {
        model_B->item(i, 2)->setTextAlignment(Qt::AlignCenter);
        model_B->item(i, 0)->setEditable(false); // 设置不可编辑
    }
}

void page_category::on_checkBox_summary_all_family_clicked()
{
    if (ui.checkBox_summary_all_family->isChecked())
    {
        m_family_user_list_selected = m_family_user_list;
        ui.pushButton_summary_selectFromFamily->setEnabled(false);
    }
    else
    {
        ui.pushButton_summary_selectFromFamily->setEnabled(true);
    }
}

void page_category::on_checkBox_summary_all_category_clicked()
{
    if (ui.checkBox_summary_all_category->isChecked())
    {
        m_category_info_selected = m_category_info;
        ui.pushButton_summary_selectFromCategoryList->setEnabled(false);
    }
    else
    {
        ui.pushButton_summary_selectFromCategoryList->setEnabled(true);
    }
}

void page_category::onUpdate_UI_under_summary()
{
    ui.label_category_edit_summary_expense->setText(QString::number(m_category_edit_expense));
    ui.label_category_edit_summary_income->setText(QString::number(m_category_edit_income));
    ui.label_category_edit_summary_all->setText(QString::number(m_category_edit_all_income));
    ui.label_category_summaryA_summary_expense->setText(QString::number(m_summary_A_edit_expense));
    ui.label_category_summaryA_summary_income->setText(QString::number(m_summary_A_edit_income));
    ui.label_category_summaryA_summary_all->setText(QString::number(m_summary_A_edit_all_income));
    ui.label_category_summaryB_summary_expense->setText(QString::number(m_summary_B_edit_expense));
    ui.label_category_summaryB_summary_income->setText(QString::number(m_summary_B_edit_income));
    ui.label_category_summaryB_summary_all->setText(QString::number(m_summary_B_edit_all_income));
}

void page_category::setupTableView()
{
    // 初始化用于编辑category的tableview
    model = new QStandardItemModel(this);
    ui.tableView->setModel(model);
    ui.tableView->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked);
    // 设置表头
    QStringList header = {"分类ID", "分类名", "项目类型", "项目金额"};
    model->setHorizontalHeaderLabels(header);
    // 设置表头自适应
    ui.tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    // 设置表头居中
    ui.tableView->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
    // 设置字体微软雅黑
    ui.tableView->horizontalHeader()->setFont(QFont("Microsoft YaHei", 14));
    ui.tableView->setFont(QFont("Microsoft YaHei", 14));
    // 隐藏第一列
    ui.tableView->setColumnHidden(0, true);
    delegate_radio *delegate = new delegate_radio(this);
    ui.tableView->setItemDelegateForColumn(2, delegate);
    connect(model, &QStandardItemModel::itemChanged, this, &page_category::onItemChanged);

    // 初始化用于分类汇总的tableview:tableView_A和tableView_B
    // tableView_A与tableView_B分别展示两个时间范围分享汇总的统计结果
    model_A = new QStandardItemModel(this);
    ui.tableView_A->setModel(model_A);
    ui.tableView_A->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // 设置表头
    QStringList header_A = {"分类ID", "分类名", "项目类型", "项目金额"};
    model_A->setHorizontalHeaderLabels(header_A);
    // 设置表头自适应
    ui.tableView_A->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    // 设置表头居中
    ui.tableView_A->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
    // 设置字体微软雅黑
    ui.tableView_A->horizontalHeader()->setFont(QFont("Microsoft YaHei", 14));
    ui.tableView_A->setFont(QFont("Microsoft YaHei", 14));
    // 隐藏第一列
    ui.tableView_A->setColumnHidden(0, true);

    model_B = new QStandardItemModel(this);
    ui.tableView_B->setModel(model_B);
    ui.tableView_B->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // 设置表头
    QStringList header_B = {"分类ID", "分类名", "项目类型", "项目金额"};
    model_B->setHorizontalHeaderLabels(header_B);
    // 设置表头自适应
    ui.tableView_B->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    // 设置表头居中
    ui.tableView_B->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
    // 设置字体微软雅黑
    ui.tableView_B->horizontalHeader()->setFont(QFont("Microsoft YaHei", 14));
    ui.tableView_B->setFont(QFont("Microsoft YaHei", 14));
    // 隐藏第一列
    ui.tableView_B->setColumnHidden(0, true);
}

void page_category::onGetCategory(QVector<category_summary> categories)
{
    m_category_info = categories; // 保存备用
    disconnect(model, &QStandardItemModel::itemChanged, this, &page_category::onItemChanged);
    model->removeRows(0, model->rowCount());

    for (int i1 = 0, count = 0; i1 < categories.size(); i1++)
    {
        category_summary category = categories[i1];
        if (ui.radioButton_selectType_all->isChecked() || (ui.radioButton_selectType_incmome->isChecked() && category.is_income) || (ui.radioButton_selectType_expense->isChecked() && !category.is_income))
        {
            QStandardItem *item0 = new QStandardItem(QString::number(category.category_id));
            QStandardItem *item1 = new QStandardItem(category.category_name);
            QStandardItem *item2 = new QStandardItem(category.is_income ? "收入" : "支出");
            QStandardItem *item3 = new QStandardItem(QString::number(category.amount));
            model->setItem(count, 0, item0);
            model->setItem(count, 1, item1);
            model->setItem(count, 2, item2);
            model->setItem(count, 3, item3);
            count++;
            if (category.is_income)
            {
                m_category_edit_income += category.amount;
            }
            else
            {
                m_category_edit_expense += category.amount;
            }
            m_category_edit_all_income = m_category_edit_income - m_category_edit_expense;
        }
    }
    emit update_UI_under_summary();
    // 设置表格居中
    for (int i = 0; i < model->rowCount(); i++)
    {
        model->item(i, 2)->setTextAlignment(Qt::AlignCenter);
        model->item(i, 0)->setEditable(false); // 设置不可编辑
    }
    connect(model, &QStandardItemModel::itemChanged, this, &page_category::onItemChanged);
}
