#include "departmentpage.h"

#include "db/dbmanager.h"
#include "db/historylogger.h"
#include "models/departmentmodel.h"
#include "ui/departmenteditdialog.h"

#include <QHeaderView>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSqlRecord>
#include <QTableView>
#include <QVBoxLayout>
#include <QUuid>

static bool insertDepartment(const QString& id, const QString& name, QString* error)
{
    return DbManager::instance().exec(QStringLiteral("INSERT INTO Department(ID,NAME) VALUES(?,?);"), {id, name}, error);
}

static bool updateDepartment(const QString& id, const QString& name, QString* error)
{
    return DbManager::instance().exec(QStringLiteral("UPDATE Department SET NAME=? WHERE ID=?;"), {name, id}, error);
}

static bool deleteDepartmentById(const QString& id, QString* error)
{
    return DbManager::instance().exec(QStringLiteral("DELETE FROM Department WHERE ID=?;"), {id}, error);
}

DepartmentPage::DepartmentPage(QWidget* parent)
    : QWidget(parent)
{
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(20, 20, 20, 20);
    root->setSpacing(12);

    auto* top = new QHBoxLayout();
    m_keyword = new QLineEdit(this);
    m_keyword->setPlaceholderText(QStringLiteral("输入科室名称关键字"));
    m_searchBtn = new QPushButton(QStringLiteral("查找"), this);
    m_addBtn = new QPushButton(QStringLiteral("添加"), this);
    m_deleteBtn = new QPushButton(QStringLiteral("删除"), this);
    m_editBtn = new QPushButton(QStringLiteral("修改"), this);

    top->addWidget(m_keyword, 1);
    top->addWidget(m_searchBtn);
    top->addWidget(m_addBtn);
    top->addWidget(m_deleteBtn);
    top->addWidget(m_editBtn);
    root->addLayout(top);

    m_model = new DepartmentModel(this);

    m_table = new QTableView(this);
    m_table->setModel(m_model);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->setAlternatingRowColors(true);
    root->addWidget(m_table, 1);

    connect(m_searchBtn, &QPushButton::clicked, this, &DepartmentPage::onSearch);
    connect(m_keyword, &QLineEdit::returnPressed, this, &DepartmentPage::onSearch);
    connect(m_addBtn, &QPushButton::clicked, this, &DepartmentPage::onAdd);
    connect(m_editBtn, &QPushButton::clicked, this, &DepartmentPage::onEdit);
    connect(m_deleteBtn, &QPushButton::clicked, this, &DepartmentPage::onDelete);
}

void DepartmentPage::setCurrentUserId(const QString& userId)
{
    m_userId = userId;
}

void DepartmentPage::onSearch()
{
    m_model->setKeywordFilter(m_keyword->text());
}

int DepartmentPage::selectedRow() const
{
    const auto idx = m_table->currentIndex();
    return idx.isValid() ? idx.row() : -1;
}

void DepartmentPage::onAdd()
{
    const auto id = QUuid::createUuid().toString(QUuid::WithoutBraces);
    DepartmentEditDialog dlg(this);
    dlg.setWindowTitle(QStringLiteral("添加科室"));
    dlg.setDepartment(id, {});
    if (dlg.exec() != QDialog::Accepted) {
        return;
    }
    const auto name = dlg.name();
    if (name.isEmpty()) {
        QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("科室名称不能为空。"));
        return;
    }
    QString err;
    if (!insertDepartment(id, name, &err)) {
        QMessageBox::critical(this, QStringLiteral("添加失败"), err);
        return;
    }
    m_model->select();
    HistoryLogger::logEvent(m_userId, QStringLiteral("添加科室：%1(%2)").arg(name, id));
}

void DepartmentPage::onEdit()
{
    const int row = selectedRow();
    if (row < 0) {
        QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("请先选择一行。"));
        return;
    }
    const auto rec = m_model->record(row);
    const auto id = rec.value(QStringLiteral("ID")).toString();
    const auto name = rec.value(QStringLiteral("NAME")).toString();

    DepartmentEditDialog dlg(this);
    dlg.setWindowTitle(QStringLiteral("编辑科室"));
    dlg.setDepartment(id, name);
    if (dlg.exec() != QDialog::Accepted) {
        return;
    }
    const auto newName = dlg.name();
    if (newName.isEmpty()) {
        QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("科室名称不能为空。"));
        return;
    }
    QString err;
    if (!updateDepartment(id, newName, &err)) {
        QMessageBox::critical(this, QStringLiteral("修改失败"), err);
        return;
    }
    m_model->select();
    HistoryLogger::logEvent(m_userId, QStringLiteral("修改科室：%1(%2)").arg(newName, id));
}

void DepartmentPage::onDelete()
{
    const int row = selectedRow();
    if (row < 0) {
        QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("请先选择一行。"));
        return;
    }
    const auto rec = m_model->record(row);
    const auto id = rec.value(QStringLiteral("ID")).toString();
    const auto name = rec.value(QStringLiteral("NAME")).toString();
    if (QMessageBox::question(this,
                             QStringLiteral("确认删除"),
                             QStringLiteral("确定删除科室：%1？").arg(name))
        != QMessageBox::Yes) {
        return;
    }

    QString err;
    if (!deleteDepartmentById(id, &err)) {
        QMessageBox::critical(this, QStringLiteral("删除失败"), err);
        return;
    }
    m_model->select();
    HistoryLogger::logEvent(m_userId, QStringLiteral("删除科室：%1(%2)").arg(name, id));
}

