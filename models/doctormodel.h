#pragma once

#include <QSqlRelationalTableModel>

class DoctorModel final : public QSqlRelationalTableModel
{
    Q_OBJECT

public:
    explicit DoctorModel(QObject* parent = nullptr);

    void setKeywordFilter(const QString& keyword);

private:
    static QString escapeLike(const QString& text);
};

