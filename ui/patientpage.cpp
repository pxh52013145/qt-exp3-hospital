#include "patientpage.h"

#include "db/dbmanager.h"
#include "db/historylogger.h"
#include "delegates/patientdelegate.h"
#include "entities/patient.h"
#include "models/patientmodel.h"
#include "ui/patienteditdialog.h"

#include <QDateTime>
#include <QHeaderView>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSqlRecord>
#include <QTableView>
#include <QVBoxLayout>
#include <QUuid>

static Patient recordToPatient(const QSqlRecord& r)
{
    Patient p;
    p.id = r.value(QStringLiteral("ID")).toString();
    p.idCard = r.value(QStringLiteral("ID_CARD")).toString();
    p.name = r.value(QStringLiteral("NAME")).toString();
    p.sex = r.value(QStringLiteral("SEX")).toInt();
    p.dob = QDate::fromString(r.value(QStringLiteral("DOB")).toString(), Qt::ISODate);
    p.height = r.value(QStringLiteral("HEIGHT")).toDouble();
    p.weight = r.value(QStringLiteral("WEIGHT")).toDouble();
    p.mobilePhone = r.value(QStringLiteral("MOBILEPHONE")).toString();
    p.age = r.value(QStringLiteral("AGE")).toInt();
    return p;
}

static bool insertPatient(const Patient& p, QString* error)
{
    const auto created = QDateTime::currentDateTime().toString(Qt::ISODate);
    return DbManager::instance().exec(
        QStringLiteral(
            "INSERT INTO Patient(ID,ID_CARD,NAME,SEX,DOB,HEIGHT,WEIGHT,MOBILEPHONE,AGE,CREATEDTIMESTAMP)"
            " VALUES(?,?,?,?,?,?,?,?,?,?);"),
        {p.id,
         p.idCard,
         p.name,
         p.sex,
         p.dob.toString(Qt::ISODate),
         p.height,
         p.weight,
         p.mobilePhone,
         p.age,
         created},
        error);
}

static bool updatePatient(const Patient& p, QString* error)
{
    return DbManager::instance().exec(
        QStringLiteral(
            "UPDATE Patient SET ID_CARD=?,NAME=?,SEX=?,DOB=?,HEIGHT=?,WEIGHT=?,MOBILEPHONE=?,AGE=? WHERE ID=?;"),
        {p.idCard,
         p.name,
         p.sex,
         p.dob.toString(Qt::ISODate),
         p.height,
         p.weight,
         p.mobilePhone,
         p.age,
         p.id},
        error);
}

static bool deletePatientById(const QString& id, QString* error)
{
    return DbManager::instance().exec(QStringLiteral("DELETE FROM Patient WHERE ID=?;"), {id}, error);
}

PatientPage::PatientPage(QWidget* parent)
    : QWidget(parent)
{
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(20, 20, 20, 20);
    root->setSpacing(12);

    auto* top = new QHBoxLayout();
    m_keyword = new QLineEdit(this);
    m_keyword->setPlaceholderText(QStringLiteral("输入身份证/姓名/手机号关键字"));
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

    m_model = new PatientModel(this);

    m_table = new QTableView(this);
    m_table->setModel(m_model);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setAlternatingRowColors(true);
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked);
    m_table->setItemDelegate(new PatientDelegate(m_model, m_table));
    root->addWidget(m_table, 1);

    connect(m_searchBtn, &QPushButton::clicked, this, &PatientPage::onSearch);
    connect(m_keyword, &QLineEdit::returnPressed, this, &PatientPage::onSearch);
    connect(m_addBtn, &QPushButton::clicked, this, &PatientPage::onAdd);
    connect(m_editBtn, &QPushButton::clicked, this, &PatientPage::onEdit);
    connect(m_deleteBtn, &QPushButton::clicked, this, &PatientPage::onDelete);
}

void PatientPage::setCurrentUserId(const QString& userId)
{
    m_userId = userId;
}

void PatientPage::onSearch()
{
    m_model->setKeywordFilter(m_keyword->text());
}

int PatientPage::selectedRow() const
{
    const auto idx = m_table->currentIndex();
    if (!idx.isValid()) {
        return -1;
    }
    return idx.row();
}

void PatientPage::onAdd()
{
    Patient p;
    p.id = QUuid::createUuid().toString(QUuid::WithoutBraces);
    p.dob = QDate::currentDate();

    PatientEditDialog dlg(this);
    dlg.setWindowTitle(QStringLiteral("添加患者信息"));
    dlg.setPatient(p);
    if (dlg.exec() != QDialog::Accepted) {
        return;
    }

    p = dlg.patient();
    if (p.name.isEmpty()) {
        QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("姓名不能为空。"));
        return;
    }

    QString err;
    if (!insertPatient(p, &err)) {
        QMessageBox::critical(this, QStringLiteral("添加失败"), err);
        return;
    }

    m_model->select();
    HistoryLogger::logEvent(m_userId, QStringLiteral("添加患者：%1(%2)").arg(p.name, p.id));
}

void PatientPage::onEdit()
{
    const int row = selectedRow();
    if (row < 0) {
        QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("请先选择一行。"));
        return;
    }

    const auto rec = m_model->record(row);
    Patient p = recordToPatient(rec);

    PatientEditDialog dlg(this);
    dlg.setWindowTitle(QStringLiteral("编辑患者信息"));
    dlg.setPatient(p);
    if (dlg.exec() != QDialog::Accepted) {
        return;
    }

    p = dlg.patient();
    if (p.name.isEmpty()) {
        QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("姓名不能为空。"));
        return;
    }

    QString err;
    if (!updatePatient(p, &err)) {
        QMessageBox::critical(this, QStringLiteral("修改失败"), err);
        return;
    }
    m_model->select();
    HistoryLogger::logEvent(m_userId, QStringLiteral("修改患者：%1(%2)").arg(p.name, p.id));
}

void PatientPage::onDelete()
{
    const int row = selectedRow();
    if (row < 0) {
        QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("请先选择一行。"));
        return;
    }

    const auto id = m_model->record(row).value(QStringLiteral("ID")).toString();
    const auto name = m_model->record(row).value(QStringLiteral("NAME")).toString();

    if (QMessageBox::question(this,
                             QStringLiteral("确认删除"),
                             QStringLiteral("确定删除患者：%1？").arg(name))
        != QMessageBox::Yes) {
        return;
    }

    QString err;
    if (!deletePatientById(id, &err)) {
        QMessageBox::critical(this, QStringLiteral("删除失败"), err);
        return;
    }
    m_model->select();
    HistoryLogger::logEvent(m_userId, QStringLiteral("删除患者：%1(%2)").arg(name, id));
}
