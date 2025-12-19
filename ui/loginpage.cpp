#include "loginpage.h"

#include "appinfo.h"
#include "db/dbmanager.h"
#include "db/historylogger.h"

#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSqlError>
#include <QSqlQuery>
#include <QVBoxLayout>
#include <QUuid>

static bool createUser(const QString& fullName,
                       const QString& username,
                       const QString& password,
                       QString* error)
{
    const auto id = QUuid::createUuid().toString(QUuid::WithoutBraces);
    return DbManager::instance().exec(
        QStringLiteral("INSERT INTO User(ID,FULLNAME,USERNAME,PASSWORD) VALUES(?,?,?,?);"),
        {id, fullName, username, password},
        error);
}

LoginPage::LoginPage(QWidget* parent)
    : QWidget(parent)
{
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(40, 40, 40, 40);
    root->setSpacing(16);

    auto* title = new QLabel(QStringLiteral("欢迎使用%1").arg(AppInfo::kAppTitle), this);
    QFont f = title->font();
    f.setPointSize(f.pointSize() + 6);
    f.setBold(true);
    title->setFont(f);
    title->setAlignment(Qt::AlignHCenter);
    root->addWidget(title);

    auto* formWrap = new QWidget(this);
    auto* form = new QFormLayout(formWrap);
    form->setLabelAlignment(Qt::AlignRight);
    form->setFormAlignment(Qt::AlignHCenter);

    m_username = new QLineEdit(this);
    m_username->setPlaceholderText(QStringLiteral("admin"));
    m_password = new QLineEdit(this);
    m_password->setEchoMode(QLineEdit::Password);
    m_password->setPlaceholderText(QStringLiteral("123456"));

    form->addRow(QStringLiteral("用户名："), m_username);
    form->addRow(QStringLiteral("密  码："), m_password);

    root->addWidget(formWrap);

    auto* btnRow = new QHBoxLayout();
    m_registerBtn = new QPushButton(QStringLiteral("注册"), this);
    m_loginBtn = new QPushButton(QStringLiteral("登录"), this);
    btnRow->addStretch(1);
    btnRow->addWidget(m_registerBtn);
    btnRow->addWidget(m_loginBtn);
    btnRow->addStretch(1);
    root->addLayout(btnRow);

    root->addStretch(1);

    connect(m_loginBtn, &QPushButton::clicked, this, &LoginPage::onLoginClicked);
    connect(m_registerBtn, &QPushButton::clicked, this, &LoginPage::onRegisterClicked);
}

void LoginPage::onLoginClicked()
{
    const auto username = m_username->text().trimmed();
    const auto password = m_password->text();
    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("请输入用户名和密码。"));
        return;
    }

    QSqlQuery q(DbManager::instance().database());
    q.prepare(QStringLiteral("SELECT ID,FULLNAME FROM User WHERE USERNAME=? AND PASSWORD=? LIMIT 1;"));
    q.addBindValue(username);
    q.addBindValue(password);
    if (!q.exec()) {
        QMessageBox::critical(this, QStringLiteral("错误"), q.lastError().text());
        return;
    }
    if (!q.next()) {
        QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("用户名或密码错误。"));
        return;
    }

    UserInfo u;
    u.id = q.value(0).toString();
    u.username = username;
    u.fullName = q.value(1).toString();

    HistoryLogger::logEvent(u.id, QStringLiteral("登录：%1").arg(username));
    emit loginSucceeded(u);
}

void LoginPage::onRegisterClicked()
{
    QDialog dlg(this);
    dlg.setWindowTitle(QStringLiteral("注册用户"));

    auto* root = new QVBoxLayout(&dlg);
    auto* form = new QFormLayout();

    QLineEdit fullName;
    QLineEdit username;
    QLineEdit password;
    password.setEchoMode(QLineEdit::Password);

    form->addRow(QStringLiteral("姓名："), &fullName);
    form->addRow(QStringLiteral("用户名："), &username);
    form->addRow(QStringLiteral("密码："), &password);
    root->addLayout(form);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Cancel | QDialogButtonBox::Ok, &dlg);
    buttons->button(QDialogButtonBox::Ok)->setText(QStringLiteral("注册"));
    buttons->button(QDialogButtonBox::Cancel)->setText(QStringLiteral("取消"));
    root->addWidget(buttons);
    connect(buttons, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    if (dlg.exec() != QDialog::Accepted) {
        return;
    }

    const auto n = fullName.text().trimmed();
    const auto u = username.text().trimmed();
    const auto p = password.text();
    if (n.isEmpty() || u.isEmpty() || p.isEmpty()) {
        QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("请填写完整信息。"));
        return;
    }

    QString err;
    if (!createUser(n, u, p, &err)) {
        QMessageBox::critical(this, QStringLiteral("注册失败"), err);
        return;
    }
    QMessageBox::information(this, QStringLiteral("成功"), QStringLiteral("注册成功，请登录。"));
}

