#pragma once

#include <QSqlTableModel>

class DepartmentModel final : public QSqlTableModel
{
    Q_OBJECT

public:
    explicit DepartmentModel(QObject* parent = nullptr);

    void setKeywordFilter(const QString& keyword);

private:
    static QString escapeLike(const QString& text);
};

