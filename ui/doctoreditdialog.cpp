#include "doctoreditdialog.h"

#include "db/dbmanager.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QSqlError>
#include <QSqlQuery>
#include <QVBoxLayout>

DoctorEditDialog::DoctorEditDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(QStringLiteral("编辑医生"));
    setModal(true);

    auto* root = new QVBoxLayout(this);
    auto* form = new QFormLayout();
    form->setLabelAlignment(Qt::AlignRight);

    m_id = new QLineEdit(this);
    m_id->setReadOnly(true);
    m_employeeNo = new QLineEdit(this);
    m_name = new QLineEdit(this);
    m_department = new QComboBox(this);

    form->addRow(QStringLiteral("ID："), m_id);
    form->addRow(QStringLiteral("工号："), m_employeeNo);
    form->addRow(QStringLiteral("姓名："), m_name);
    form->addRow(QStringLiteral("科室："), m_department);
    root->addLayout(form);

    reloadDepartments();

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel, this);
    buttons->button(QDialogButtonBox::Save)->setText(QStringLiteral("保存"));
    buttons->button(QDialogButtonBox::Cancel)->setText(QStringLiteral("取消"));
    root->addWidget(buttons);

    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

void DoctorEditDialog::reloadDepartments()
{
    m_department->clear();
    m_department->addItem(QStringLiteral("（无）"), {});

    QSqlQuery q(DbManager::instance().database());
    if (!q.exec(QStringLiteral("SELECT ID,NAME FROM Department ORDER BY NAME;"))) {
        return;
    }
    while (q.next()) {
        m_department->addItem(q.value(1).toString(), q.value(0).toString());
    }
}

void DoctorEditDialog::setDoctor(const QString& id,
                                 const QString& employeeNo,
                                 const QString& name,
                                 const QString& departmentId)
{
    reloadDepartments();
    m_id->setText(id);
    m_employeeNo->setText(employeeNo);
    m_name->setText(name);

    const int idx = m_department->findData(departmentId);
    m_department->setCurrentIndex(idx >= 0 ? idx : 0);
}

QString DoctorEditDialog::id() const { return m_id->text().trimmed(); }
QString DoctorEditDialog::employeeNo() const { return m_employeeNo->text().trimmed(); }
QString DoctorEditDialog::name() const { return m_name->text().trimmed(); }
QString DoctorEditDialog::departmentId() const { return m_department->currentData().toString(); }
