#include "homepage.h"

#include "appinfo.h"

#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

static QPushButton* bigButton(const QString& text, QWidget* parent)
{
    auto* b = new QPushButton(text, parent);
    b->setMinimumSize(180, 120);
    return b;
}

HomePage::HomePage(QWidget* parent)
    : QWidget(parent)
{
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(40, 30, 40, 30);
    root->setSpacing(20);

    auto* title = new QLabel(QStringLiteral("欢迎"), this);
    QFont f = title->font();
    f.setPointSize(f.pointSize() + 6);
    f.setBold(true);
    title->setFont(f);
    title->setAlignment(Qt::AlignHCenter);
    root->addWidget(title);

    auto* grid = new QGridLayout();
    grid->setHorizontalSpacing(40);
    grid->setVerticalSpacing(40);
    grid->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    m_departmentBtn = bigButton(QStringLiteral("科室管理"), this);
    m_doctorBtn = bigButton(QStringLiteral("医生管理"), this);
    m_patientBtn = bigButton(QStringLiteral("患者管理"), this);

    grid->addWidget(m_departmentBtn, 0, 0);
    grid->addWidget(m_doctorBtn, 0, 1);
    grid->addWidget(m_patientBtn, 0, 2);

    root->addLayout(grid, 1);

    auto* hint = new QLabel(QStringLiteral("提示：默认账号 admin / 123456"), this);
    hint->setAlignment(Qt::AlignHCenter);
    root->addWidget(hint);

    connect(m_patientBtn, &QPushButton::clicked, this, &HomePage::openPatients);
    connect(m_doctorBtn, &QPushButton::clicked, this, &HomePage::openDoctors);
    connect(m_departmentBtn, &QPushButton::clicked, this, &HomePage::openDepartments);
}

