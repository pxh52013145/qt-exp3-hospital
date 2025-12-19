#pragma once

#include <QMainWindow>

#include "entities/userinfo.h"

class QAction;
class QLabel;
class QStackedWidget;
class HomePage;
class LoginPage;
class PatientPage;
class DoctorPage;
class DepartmentPage;
class HistoryPage;
class QToolButton;

class MainWindow final : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    enum class Page { Login, Home, Patients, Doctors, Departments, History };

    void buildUi();
    void setPage(Page p);
    void updateChrome();

    void onLogout();

    Page m_page = Page::Login;
    UserInfo m_user;

    QStackedWidget* m_stack = nullptr;
    LoginPage* m_login = nullptr;
    HomePage* m_home = nullptr;
    PatientPage* m_patients = nullptr;
    DoctorPage* m_doctors = nullptr;
    DepartmentPage* m_departments = nullptr;
    HistoryPage* m_history = nullptr;

    QAction* m_backAction = nullptr;
    QAction* m_historyAction = nullptr;
    QAction* m_logoutAction = nullptr;
    QLabel* m_statusLabel = nullptr;
    QLabel* m_titleLabel = nullptr;
    QToolButton* m_backButton = nullptr;
    QToolButton* m_userMenuButton = nullptr;
};
