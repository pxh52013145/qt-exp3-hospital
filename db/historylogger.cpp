#include "historylogger.h"

#include "db/dbmanager.h"

#include <QDateTime>

void HistoryLogger::logEvent(const QString& userId, const QString& event)
{
    const auto ts = QDateTime::currentDateTime().toString(Qt::ISODate);
    DbManager::instance().exec(QStringLiteral("INSERT INTO History(USER_ID,EVENT,TIMESTAMP) VALUES(?,?,?);"),
                               {userId, event, ts},
                               nullptr);
}

