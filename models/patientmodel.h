#pragma once

#include <QSqlTableModel>

class PatientModel final : public QSqlTableModel
{
    Q_OBJECT

public:
    explicit PatientModel(QObject* parent = nullptr);

    void setKeywordFilter(const QString& keyword);

    int sexColumn() const;
    int dobColumn() const;
    int heightColumn() const;
    int weightColumn() const;
    int ageColumn() const;

private:
    int columnIndex(const QString& fieldName) const;
    static QString escapeLike(const QString& text);
};

