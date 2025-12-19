#include "mainwindow.h"

#include "appinfo.h"
#include "ui/homepage.h"
#include "ui/loginpage.h"
#include "ui/doctorpage.h"
#include "ui/departmentpage.h"
#include "ui/historypage.h"
#include "ui/patientpage.h"

#include <QAction>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QStackedLayout>
#include <QStackedWidget>
#include <QStatusBar>
#include <QStyle>
#include <QToolButton>
#include <QToolBar>
#include <QSizePolicy>
#include <QWidget>
#include <QMenu>

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

    m_backAction = new QAction(QIcon(QStringLiteral(":/icons/back.svg")), QStringLiteral("返回"), this);
    m_historyAction = new QAction(QIcon(QStringLiteral(":/icons/history.svg")), QStringLiteral("日志"), this);
    m_logoutAction = new QAction(QIcon(QStringLiteral(":/icons/logout.svg")), QStringLiteral("退出登录"), this);

    auto* menu = new QMenu(this);
    menu->addAction(m_historyAction);
    menu->addSeparator();
    menu->addAction(m_logoutAction);

    auto* header = new QWidget(tb);
    header->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    auto* stacked = new QStackedLayout(header);
    stacked->setContentsMargins(0, 0, 0, 0);
    stacked->setStackingMode(QStackedLayout::StackAll);

    auto* row = new QWidget(header);
    auto* rowLayout = new QHBoxLayout(row);
    rowLayout->setContentsMargins(0, 0, 0, 0);
    rowLayout->setSpacing(8);

    m_backButton = new QToolButton(row);
    m_backButton->setDefaultAction(m_backAction);
    m_backButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    m_userMenuButton = new QToolButton(row);
    m_userMenuButton->setPopupMode(QToolButton::InstantPopup);
    m_userMenuButton->setMenu(menu);
    m_userMenuButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    m_userMenuButton->setIcon(style()->standardIcon(QStyle::SP_TitleBarMenuButton));
    m_userMenuButton->setText(QStringLiteral("菜单"));

    rowLayout->addWidget(m_backButton);
    rowLayout->addStretch(1);
    rowLayout->addWidget(m_userMenuButton);

    m_titleLabel = new QLabel(header);
    m_titleLabel->setAlignment(Qt::AlignCenter);
    m_titleLabel->setStyleSheet(QStringLiteral("font-weight: 600;"));
    m_titleLabel->setAttribute(Qt::WA_TransparentForMouseEvents, true);

    stacked->addWidget(row);
    stacked->addWidget(m_titleLabel);
    stacked->setAlignment(m_titleLabel, Qt::AlignCenter);

    tb->addWidget(header);

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
    m_backButton->setVisible(loggedIn);
    m_backButton->setEnabled(m_page != Page::Home);
    m_userMenuButton->setVisible(loggedIn);

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
