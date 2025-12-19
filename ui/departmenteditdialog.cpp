#include "departmenteditdialog.h"

#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

DepartmentEditDialog::DepartmentEditDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(QStringLiteral("编辑科室"));
    setModal(true);

    auto* root = new QVBoxLayout(this);
    auto* form = new QFormLayout();
    form->setLabelAlignment(Qt::AlignRight);

    m_id = new QLineEdit(this);
    m_id->setReadOnly(true);
    m_name = new QLineEdit(this);

    form->addRow(QStringLiteral("ID："), m_id);
    form->addRow(QStringLiteral("名称："), m_name);
    root->addLayout(form);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel, this);
    buttons->button(QDialogButtonBox::Save)->setText(QStringLiteral("保存"));
    buttons->button(QDialogButtonBox::Cancel)->setText(QStringLiteral("取消"));
    root->addWidget(buttons);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

void DepartmentEditDialog::setDepartment(const QString& id, const QString& name)
{
    m_id->setText(id);
    m_name->setText(name);
}

QString DepartmentEditDialog::id() const
{
    return m_id->text().trimmed();
}

QString DepartmentEditDialog::name() const
{
    return m_name->text().trimmed();
}
