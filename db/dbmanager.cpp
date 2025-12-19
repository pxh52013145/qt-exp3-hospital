#include "dbmanager.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QStandardPaths>

static QString lastSqlError(const QSqlQuery& query)
{
    const auto err = query.lastError();
    if (err.isValid()) {
        return err.text();
    }
    return {};
}

DbManager& DbManager::instance()
{
    static DbManager inst;
    return inst;
}

QString DbManager::databasePath() const
{
    const auto base = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(base);
    return QDir(base).filePath(QStringLiteral("hospital.db"));
}

bool DbManager::open(QString* error)
{
    const QString connectionName = QStringLiteral("main");
    if (QSqlDatabase::contains(connectionName)) {
        m_db = QSqlDatabase::database(connectionName);
    } else {
        m_db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), connectionName);
        m_db.setDatabaseName(databasePath());
    }

    if (!m_db.open()) {
        if (error) {
            *error = m_db.lastError().text();
        }
        return false;
    }

    QSqlQuery pragma(m_db);
    pragma.exec(QStringLiteral("PRAGMA foreign_keys = ON;"));

    if (!ensureSchema(error)) {
        return false;
    }
    return seedDefaultUser(error);
}

QSqlDatabase DbManager::database() const
{
    return m_db;
}

bool DbManager::exec(const QString& sql, const QVariantList& args, QString* error) const
{
    QSqlQuery query(m_db);
    if (!query.prepare(sql)) {
        if (error) {
            *error = query.lastError().text();
        }
        return false;
    }
    for (const auto& v : args) {
        query.addBindValue(v);
    }
    if (!query.exec()) {
        if (error) {
            *error = lastSqlError(query);
        }
        return false;
    }
    return true;
}

QSqlQueryModel* DbManager::createQueryModel(const QString& sql,
                                           const QVariantList& args,
                                           QObject* parent,
                                           QString* error) const
{
    auto* model = new QSqlQueryModel(parent);
    QSqlQuery query(m_db);
    if (!query.prepare(sql)) {
        if (error) {
            *error = query.lastError().text();
        }
        model->setQuery(QSqlQuery());
        return model;
    }
    for (const auto& v : args) {
        query.addBindValue(v);
    }
    if (!query.exec()) {
        if (error) {
            *error = lastSqlError(query);
        }
        model->setQuery(QSqlQuery());
        return model;
    }
    model->setQuery(query);
    return model;
}

bool DbManager::ensureSchema(QString* error) const
{
    const QStringList statements = {
        QStringLiteral(
            "CREATE TABLE IF NOT EXISTS User ("
            "  ID TEXT PRIMARY KEY,"
            "  FULLNAME TEXT,"
            "  USERNAME TEXT UNIQUE,"
            "  PASSWORD TEXT"
            ");"),
        QStringLiteral(
            "CREATE TABLE IF NOT EXISTS Patient ("
            "  ID TEXT PRIMARY KEY,"
            "  ID_CARD TEXT,"
            "  NAME TEXT,"
            "  SEX INTEGER,"
            "  DOB TEXT,"
            "  HEIGHT REAL,"
            "  WEIGHT REAL,"
            "  MOBILEPHONE TEXT,"
            "  AGE INTEGER,"
            "  CREATEDTIMESTAMP TEXT"
            ");"),
        QStringLiteral(
            "CREATE TABLE IF NOT EXISTS Department ("
            "  ID TEXT PRIMARY KEY,"
            "  NAME TEXT"
            ");"),
        QStringLiteral(
            "CREATE TABLE IF NOT EXISTS Doctor ("
            "  ID TEXT PRIMARY KEY,"
            "  EMPLOYEENO TEXT,"
            "  NAME TEXT,"
            "  DEPARTMENT_ID TEXT,"
            "  FOREIGN KEY(DEPARTMENT_ID) REFERENCES Department(ID)"
            "    ON UPDATE CASCADE ON DELETE SET NULL"
            ");"),
        QStringLiteral(
            "CREATE TABLE IF NOT EXISTS History ("
            "  ID INTEGER PRIMARY KEY AUTOINCREMENT,"
            "  USER_ID TEXT,"
            "  EVENT TEXT,"
            "  TIMESTAMP TEXT,"
            "  FOREIGN KEY(USER_ID) REFERENCES User(ID)"
            "    ON UPDATE CASCADE ON DELETE SET NULL"
            ");"),
    };

    for (const auto& sql : statements) {
        if (!exec(sql, {}, error)) {
            return false;
        }
    }
    return true;
}

bool DbManager::seedDefaultUser(QString* error) const
{
    QSqlQuery query(m_db);
    if (!query.prepare(QStringLiteral("SELECT COUNT(1) FROM User WHERE USERNAME=?;"))) {
        if (error) {
            *error = query.lastError().text();
        }
        return false;
    }
    query.addBindValue(QStringLiteral("admin"));
    if (!query.exec()) {
        if (error) {
            *error = lastSqlError(query);
        }
        return false;
    }
    int count = 0;
    if (query.next()) {
        count = query.value(0).toInt();
    }
    if (count > 0) {
        return true;
    }

    const QString id = QStringLiteral("u-admin");
    return exec(QStringLiteral("INSERT INTO User(ID,FULLNAME,USERNAME,PASSWORD) VALUES(?,?,?,?);"),
                {id, QStringLiteral("管理员"), QStringLiteral("admin"), QStringLiteral("123456")},
                error);
}

