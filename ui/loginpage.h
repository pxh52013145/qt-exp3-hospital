#pragma once

#include <QWidget>

#include "entities/userinfo.h"

class QLineEdit;
class QPushButton;

class LoginPage final : public QWidget
{
    Q_OBJECT

public:
    explicit LoginPage(QWidget* parent = nullptr);

signals:
    void loginSucceeded(const UserInfo& user);

private:
    void onLoginClicked();
    void onRegisterClicked();

    QLineEdit* m_username = nullptr;
    QLineEdit* m_password = nullptr;
    QPushButton* m_registerBtn = nullptr;
    QPushButton* m_loginBtn = nullptr;
};

