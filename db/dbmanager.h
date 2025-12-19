#pragma once

#include <QSqlDatabase>
#include <QString>
#include <QVariant>
#include <QVariantList>

class QSqlQueryModel;

class DbManager final
{
public:
    DbManager(const DbManager&) = delete;
    DbManager& operator=(const DbManager&) = delete;

    static DbManager& instance();

    bool open(QString* error = nullptr);
    QSqlDatabase database() const;

    bool exec(const QString& sql, const QVariantList& args = {}, QString* error = nullptr) const;
    QSqlQueryModel* createQueryModel(const QString& sql,
                                    const QVariantList& args = {},
                                    QObject* parent = nullptr,
                                    QString* error = nullptr) const;

private:
    DbManager() = default;

    bool ensureSchema(QString* error) const;
    bool seedDefaultUser(QString* error) const;
    bool seedDemoData(QString* error) const;

    QString databasePath() const;

    mutable QSqlDatabase m_db;
};
