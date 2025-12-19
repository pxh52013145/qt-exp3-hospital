#pragma once

#include <QString>

class HistoryLogger final
{
public:
    static void logEvent(const QString& userId, const QString& event);
};

