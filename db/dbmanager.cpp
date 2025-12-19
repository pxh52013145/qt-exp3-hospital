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
    if (!seedDefaultUser(error)) {
        return false;
    }
    return seedDemoData(error);
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

static int scalarCount(QSqlDatabase db, const QString& sql, const QVariantList& args, QString* error)
{
    QSqlQuery q(db);
    if (!q.prepare(sql)) {
        if (error) {
            *error = q.lastError().text();
        }
        return -1;
    }
    for (const auto& v : args) {
        q.addBindValue(v);
    }
    if (!q.exec()) {
        if (error) {
            *error = q.lastError().text();
        }
        return -1;
    }
    if (!q.next()) {
        return 0;
    }
    return q.value(0).toInt();
}

bool DbManager::seedDemoData(QString* error) const
{
    // 仅在空表时插入演示数据，避免重复污染。
    const int deptCount = scalarCount(m_db, QStringLiteral("SELECT COUNT(1) FROM Department;"), {}, error);
    if (deptCount < 0) {
        return false;
    }
    const int doctorCount = scalarCount(m_db, QStringLiteral("SELECT COUNT(1) FROM Doctor;"), {}, error);
    if (doctorCount < 0) {
        return false;
    }
    const int patientCount = scalarCount(m_db, QStringLiteral("SELECT COUNT(1) FROM Patient;"), {}, error);
    if (patientCount < 0) {
        return false;
    }

    const int hasSimpleDeptIds = scalarCount(m_db,
                                            QStringLiteral("SELECT COUNT(1) FROM Department WHERE ID IN ('ks1','ks2','ks3','ks4');"),
                                            {},
                                            error);
    if (hasSimpleDeptIds < 0) {
        return false;
    }

    if (deptCount == 0 || hasSimpleDeptIds == 0) {
        const struct {
            const char* id;
            const char* name;
        } depts[] = {
            {"ks1", "内科"},
            {"ks2", "外科"},
            {"ks3", "儿科"},
            {"ks4", "急诊科"},
        };
        for (const auto& d : depts) {
            if (!exec(QStringLiteral("INSERT OR IGNORE INTO Department(ID,NAME) VALUES(?,?);"),
                      {QString::fromUtf8(d.id), QString::fromUtf8(d.name)},
                      error)) {
                return false;
            }
        }
    }

    const int hasSimpleDoctorIds = scalarCount(m_db,
                                              QStringLiteral("SELECT COUNT(1) FROM Doctor WHERE ID IN ('ys1','ys2','ys3','ys4');"),
                                              {},
                                              error);
    if (hasSimpleDoctorIds < 0) {
        return false;
    }

    if (doctorCount == 0 || hasSimpleDoctorIds == 0) {
        const struct {
            const char* id;
            const char* emp;
            const char* name;
            const char* deptId;
        } doctors[] = {
            {"ys1", "YS001", "张医生", "ks1"},
            {"ys2", "YS002", "李医生", "ks2"},
            {"ys3", "YS003", "王医生", "ks3"},
            {"ys4", "YS004", "赵医生", "ks4"},
        };
        for (const auto& d : doctors) {
            if (!exec(QStringLiteral("INSERT OR IGNORE INTO Doctor(ID,EMPLOYEENO,NAME,DEPARTMENT_ID) VALUES(?,?,?,?);"),
                      {QString::fromUtf8(d.id),
                       QString::fromUtf8(d.emp),
                       QString::fromUtf8(d.name),
                       QString::fromUtf8(d.deptId)},
                      error)) {
                return false;
            }
        }
    }

    const int hasSimplePatientIds = scalarCount(m_db,
                                               QStringLiteral("SELECT COUNT(1) FROM Patient WHERE ID IN ('hz1','hz2','hz3','hz4');"),
                                               {},
                                               error);
    if (hasSimplePatientIds < 0) {
        return false;
    }

    if (patientCount == 0 || hasSimplePatientIds == 0) {
        const auto created = QDateTime::currentDateTime().toString(Qt::ISODate);
        const struct {
            const char* id;
            const char* idCard;
            const char* name;
            int sex;
            const char* dob;
            double height;
            double weight;
            const char* mobile;
            int age;
        } patients[] = {
            {"hz1", "110101199801010011", "张三", 1, "1998-01-01", 175.2, 70.5, "13800000001", 27},
            {"hz2", "110101200203050022", "李四", 0, "2002-03-05", 162.0, 52.0, "13800000002", 23},
            {"hz3", "110101198912120033", "王五", 1, "1989-12-12", 180.0, 82.3, "13800000003", 35},
            {"hz4", "110101201506300044", "赵六", 0, "2015-06-30", 120.5, 25.0, "13800000004", 10},
        };
        for (const auto& p : patients) {
            if (!exec(QStringLiteral(
                          "INSERT OR IGNORE INTO Patient(ID,ID_CARD,NAME,SEX,DOB,HEIGHT,WEIGHT,MOBILEPHONE,AGE,CREATEDTIMESTAMP)"
                          " VALUES(?,?,?,?,?,?,?,?,?,?);"),
                      {QString::fromUtf8(p.id),
                       QString::fromUtf8(p.idCard),
                       QString::fromUtf8(p.name),
                       p.sex,
                       QString::fromUtf8(p.dob),
                       p.height,
                       p.weight,
                       QString::fromUtf8(p.mobile),
                       p.age,
                       created},
                      error)) {
                return false;
            }
        }
    }

    return true;
}
