#include "patienteditdialog.h"

#include <QComboBox>
#include <QDateEdit>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>

PatientEditDialog::PatientEditDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(QStringLiteral("编辑患者信息"));
    setModal(true);

    auto* root = new QVBoxLayout(this);
    auto* form = new QFormLayout();
    form->setLabelAlignment(Qt::AlignRight);

    m_id = new QLineEdit(this);
    m_id->setReadOnly(true);
    m_name = new QLineEdit(this);
    m_idCard = new QLineEdit(this);

    m_sex = new QComboBox(this);
    m_sex->addItem(QStringLiteral("女"), 0);
    m_sex->addItem(QStringLiteral("男"), 1);

    m_dob = new QDateEdit(this);
    m_dob->setCalendarPopup(true);
    m_dob->setDisplayFormat(QStringLiteral("yyyy/M/d"));

    m_height = new QDoubleSpinBox(this);
    m_height->setRange(0.0, 250.0);
    m_height->setDecimals(1);

    m_weight = new QDoubleSpinBox(this);
    m_weight->setRange(0.0, 300.0);
    m_weight->setDecimals(1);

    m_mobile = new QLineEdit(this);

    m_age = new QSpinBox(this);
    m_age->setRange(0, 150);

    form->addRow(QStringLiteral("ID："), m_id);
    form->addRow(QStringLiteral("姓名："), m_name);
    form->addRow(QStringLiteral("身份证："), m_idCard);
    form->addRow(QStringLiteral("性别："), m_sex);
    form->addRow(QStringLiteral("出生日期："), m_dob);
    form->addRow(QStringLiteral("身高："), m_height);
    form->addRow(QStringLiteral("体重："), m_weight);
    form->addRow(QStringLiteral("手机号："), m_mobile);
    form->addRow(QStringLiteral("年龄："), m_age);

    root->addLayout(form);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel, this);
    buttons->button(QDialogButtonBox::Save)->setText(QStringLiteral("保存"));
    buttons->button(QDialogButtonBox::Cancel)->setText(QStringLiteral("取消"));
    root->addWidget(buttons);

    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

void PatientEditDialog::setPatient(const Patient& p)
{
    m_id->setText(p.id);
    m_name->setText(p.name);
    m_idCard->setText(p.idCard);
    m_sex->setCurrentIndex(p.sex == 1 ? 1 : 0);
    m_dob->setDate(p.dob.isValid() ? p.dob : QDate::currentDate());
    m_height->setValue(p.height);
    m_weight->setValue(p.weight);
    m_mobile->setText(p.mobilePhone);
    m_age->setValue(p.age);
}

Patient PatientEditDialog::patient() const
{
    Patient p;
    p.id = m_id->text().trimmed();
    p.name = m_name->text().trimmed();
    p.idCard = m_idCard->text().trimmed();
    p.sex = m_sex->currentData().toInt();
    p.dob = m_dob->date();
    p.height = m_height->value();
    p.weight = m_weight->value();
    p.mobilePhone = m_mobile->text().trimmed();
    p.age = m_age->value();
    return p;
}
