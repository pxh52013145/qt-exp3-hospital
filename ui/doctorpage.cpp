#include "doctorpage.h"

#include "db/dbmanager.h"
#include "db/historylogger.h"
#include "models/doctormodel.h"
#include "ui/doctoreditdialog.h"

#include <QHeaderView>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSqlRecord>
#include <QSqlRelationalDelegate>
#include <QTableView>
#include <QVBoxLayout>
#include <QUuid>

static bool insertDoctor(const QString& id,
                         const QString& employeeNo,
                         const QString& name,
                         const QString& departmentId,
                         QString* error)
{
    return DbManager::instance().exec(QStringLiteral("INSERT INTO Doctor(ID,EMPLOYEENO,NAME,DEPARTMENT_ID) VALUES(?,?,?,?);"),
                                      {id, employeeNo, name, departmentId.isEmpty() ? QVariant() : QVariant(departmentId)},
                                      error);
}

static bool updateDoctor(const QString& id,
                         const QString& employeeNo,
                         const QString& name,
                         const QString& departmentId,
                         QString* error)
{
    return DbManager::instance().exec(QStringLiteral("UPDATE Doctor SET EMPLOYEENO=?,NAME=?,DEPARTMENT_ID=? WHERE ID=?;"),
                                      {employeeNo,
                                       name,
                                       departmentId.isEmpty() ? QVariant() : QVariant(departmentId),
                                       id},
                                      error);
}

static bool deleteDoctorById(const QString& id, QString* error)
{
    return DbManager::instance().exec(QStringLiteral("DELETE FROM Doctor WHERE ID=?;"), {id}, error);
}

DoctorPage::DoctorPage(QWidget* parent)
    : QWidget(parent)
{
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(20, 20, 20, 20);
    root->setSpacing(12);

    auto* top = new QHBoxLayout();
    m_keyword = new QLineEdit(this);
    m_keyword->setPlaceholderText(QStringLiteral("输入工号/姓名关键字"));
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

    m_model = new DoctorModel(this);

    m_table = new QTableView(this);
    m_table->setModel(m_model);
    m_table->setItemDelegate(new QSqlRelationalDelegate(m_table));
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->setAlternatingRowColors(true);
    root->addWidget(m_table, 1);

    connect(m_searchBtn, &QPushButton::clicked, this, &DoctorPage::onSearch);
    connect(m_keyword, &QLineEdit::returnPressed, this, &DoctorPage::onSearch);
    connect(m_addBtn, &QPushButton::clicked, this, &DoctorPage::onAdd);
    connect(m_editBtn, &QPushButton::clicked, this, &DoctorPage::onEdit);
    connect(m_deleteBtn, &QPushButton::clicked, this, &DoctorPage::onDelete);
}

void DoctorPage::setCurrentUserId(const QString& userId)
{
    m_userId = userId;
}

void DoctorPage::onSearch()
{
    m_model->setKeywordFilter(m_keyword->text());
}

int DoctorPage::selectedRow() const
{
    const auto idx = m_table->currentIndex();
    return idx.isValid() ? idx.row() : -1;
}

void DoctorPage::onAdd()
{
    const auto id = QUuid::createUuid().toString(QUuid::WithoutBraces);
    DoctorEditDialog dlg(this);
    dlg.setWindowTitle(QStringLiteral("添加医生"));
    dlg.setDoctor(id, {}, {}, {});
    if (dlg.exec() != QDialog::Accepted) {
        return;
    }
    if (dlg.name().isEmpty()) {
        QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("姓名不能为空。"));
        return;
    }
    QString err;
    if (!insertDoctor(id, dlg.employeeNo(), dlg.name(), dlg.departmentId(), &err)) {
        QMessageBox::critical(this, QStringLiteral("添加失败"), err);
        return;
    }
    m_model->select();
    HistoryLogger::logEvent(m_userId, QStringLiteral("添加医生：%1(%2)").arg(dlg.name(), id));
}

void DoctorPage::onEdit()
{
    const int row = selectedRow();
    if (row < 0) {
        QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("请先选择一行。"));
        return;
    }
    const auto rec = m_model->record(row);
    const auto id = rec.value(QStringLiteral("ID")).toString();
    const auto employeeNo = rec.value(QStringLiteral("EMPLOYEENO")).toString();
    const auto name = rec.value(QStringLiteral("NAME")).toString();
    const auto departmentId = rec.value(QStringLiteral("DEPARTMENT_ID")).toString();

    DoctorEditDialog dlg(this);
    dlg.setWindowTitle(QStringLiteral("编辑医生"));
    dlg.setDoctor(id, employeeNo, name, departmentId);
    if (dlg.exec() != QDialog::Accepted) {
        return;
    }
    if (dlg.name().isEmpty()) {
        QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("姓名不能为空。"));
        return;
    }
    QString err;
    if (!updateDoctor(id, dlg.employeeNo(), dlg.name(), dlg.departmentId(), &err)) {
        QMessageBox::critical(this, QStringLiteral("修改失败"), err);
        return;
    }
    m_model->select();
    HistoryLogger::logEvent(m_userId, QStringLiteral("修改医生：%1(%2)").arg(dlg.name(), id));
}

void DoctorPage::onDelete()
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
                             QStringLiteral("确定删除医生：%1？").arg(name))
        != QMessageBox::Yes) {
        return;
    }
    QString err;
    if (!deleteDoctorById(id, &err)) {
        QMessageBox::critical(this, QStringLiteral("删除失败"), err);
        return;
    }
    m_model->select();
    HistoryLogger::logEvent(m_userId, QStringLiteral("删除医生：%1(%2)").arg(name, id));
}
