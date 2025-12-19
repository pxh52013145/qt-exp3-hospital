#include "mainwindow.h"

#include "appinfo.h"
#include "ui/homepage.h"
#include "ui/loginpage.h"
#include "ui/doctorpage.h"
#include "ui/departmentpage.h"
#include "ui/historypage.h"
#include "ui/patientpage.h"

#include <QAction>
#include <QLabel>
#include <QMessageBox>
#include <QStackedWidget>
#include <QStatusBar>
#include <QStyle>
#include <QToolBar>
#include <QSizePolicy>
#include <QWidget>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle(QStringLiteral("%1 - %2 %3").arg(AppInfo::kAppTitle, AppInfo::kStudentId, AppInfo::kStudentName));
    resize(1000, 650);
    buildUi();
    setPage(Page::Login);
}

void MainWindow::buildUi()
{
    m_stack = new QStackedWidget(this);
    setCentralWidget(m_stack);

    m_login = new LoginPage(this);
    m_home = new HomePage(this);
    m_patients = new PatientPage(this);
    m_doctors = new DoctorPage(this);
    m_departments = new DepartmentPage(this);
    m_history = new HistoryPage(this);

    m_stack->addWidget(m_login);
    m_stack->addWidget(m_home);
    m_stack->addWidget(m_patients);
    m_stack->addWidget(m_doctors);
    m_stack->addWidget(m_departments);
    m_stack->addWidget(m_history);

    auto* tb = addToolBar(QStringLiteral("Main"));
    tb->setMovable(false);
    tb->setFloatable(false);
    tb->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    m_backAction = tb->addAction(style()->standardIcon(QStyle::SP_ArrowBack), QStringLiteral("返回"));

    auto* leftSpacer = new QWidget(this);
    leftSpacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    tb->addWidget(leftSpacer);

    m_titleLabel = new QLabel(this);
    m_titleLabel->setAlignment(Qt::AlignCenter);
    m_titleLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    m_titleLabel->setStyleSheet(QStringLiteral("font-weight: 600;"));
    tb->addWidget(m_titleLabel);

    auto* rightSpacer = new QWidget(this);
    rightSpacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    tb->addWidget(rightSpacer);

    m_historyAction = tb->addAction(style()->standardIcon(QStyle::SP_FileDialogInfoView), QStringLiteral("日志"));
    m_logoutAction = tb->addAction(style()->standardIcon(QStyle::SP_DialogCloseButton), QStringLiteral("退出登录"));

    connect(m_backAction, &QAction::triggered, this, [this] { setPage(Page::Home); });
    connect(m_historyAction, &QAction::triggered, this, [this] { setPage(Page::History); });
    connect(m_logoutAction, &QAction::triggered, this, &MainWindow::onLogout);

    m_statusLabel = new QLabel(this);
    statusBar()->addPermanentWidget(m_statusLabel);

    connect(m_login, &LoginPage::loginSucceeded, this, [this](const UserInfo& u) {
        m_user = u;
        m_patients->setCurrentUserId(u.id);
        m_doctors->setCurrentUserId(u.id);
        m_departments->setCurrentUserId(u.id);
        setPage(Page::Home);
    });
    connect(m_home, &HomePage::openPatients, this, [this] { setPage(Page::Patients); });
    connect(m_home, &HomePage::openDoctors, this, [this] { setPage(Page::Doctors); });
    connect(m_home, &HomePage::openDepartments, this, [this] { setPage(Page::Departments); });
}

void MainWindow::setPage(Page p)
{
    m_page = p;
    switch (p) {
    case Page::Login:
        m_stack->setCurrentWidget(m_login);
        break;
    case Page::Home:
        m_stack->setCurrentWidget(m_home);
        break;
    case Page::Patients:
        m_stack->setCurrentWidget(m_patients);
        break;
    case Page::Doctors:
        m_stack->setCurrentWidget(m_doctors);
        break;
    case Page::Departments:
        m_stack->setCurrentWidget(m_departments);
        break;
    case Page::History:
        m_history->refresh();
        m_stack->setCurrentWidget(m_history);
        break;
    }
    updateChrome();
}

void MainWindow::updateChrome()
{
    const bool loggedIn = (m_page != Page::Login);
    m_backAction->setVisible(loggedIn);
    m_backAction->setEnabled(m_page != Page::Home);
    m_historyAction->setVisible(loggedIn);
    m_logoutAction->setVisible(loggedIn);

    switch (m_page) {
    case Page::Login:
        m_titleLabel->setText(QStringLiteral("登录"));
        break;
    case Page::Home:
        m_titleLabel->setText(QStringLiteral("欢迎"));
        break;
    case Page::Patients:
        m_titleLabel->setText(QStringLiteral("患者管理"));
        break;
    case Page::Doctors:
        m_titleLabel->setText(QStringLiteral("医生管理"));
        break;
    case Page::Departments:
        m_titleLabel->setText(QStringLiteral("科室管理"));
        break;
    case Page::History:
        m_titleLabel->setText(QStringLiteral("日志"));
        break;
    }

    const auto userText = loggedIn
        ? QStringLiteral("当前用户：%1（%2）").arg(m_user.fullName.isEmpty() ? m_user.username : m_user.fullName, m_user.username)
        : QStringLiteral("未登录");
    m_statusLabel->setText(QStringLiteral("%1 | 学号：%2 姓名：%3")
                               .arg(userText, AppInfo::kStudentId, AppInfo::kStudentName));
}

void MainWindow::onLogout()
{
    if (QMessageBox::question(this, QStringLiteral("确认"), QStringLiteral("确定退出登录？")) != QMessageBox::Yes) {
        return;
    }
    m_user = {};
    setPage(Page::Login);
}
