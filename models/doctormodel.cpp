#include "doctormodel.h"

#include <QSqlRelation>

DoctorModel::DoctorModel(QObject* parent)
    : QSqlRelationalTableModel(parent)
{
    setTable(QStringLiteral("Doctor"));
    setEditStrategy(QSqlTableModel::OnFieldChange);

    const int deptCol = fieldIndex(QStringLiteral("DEPARTMENT_ID"));
    if (deptCol >= 0) {
        setRelation(deptCol, QSqlRelation(QStringLiteral("Department"), QStringLiteral("ID"), QStringLiteral("NAME")));
    }

    select();

    setHeaderData(fieldIndex(QStringLiteral("ID")), Qt::Horizontal, QStringLiteral("ID"));
    setHeaderData(fieldIndex(QStringLiteral("EMPLOYEENO")), Qt::Horizontal, QStringLiteral("工号"));
    setHeaderData(fieldIndex(QStringLiteral("NAME")), Qt::Horizontal, QStringLiteral("姓名"));
    setHeaderData(fieldIndex(QStringLiteral("DEPARTMENT_ID")), Qt::Horizontal, QStringLiteral("科室"));
}

QString DoctorModel::escapeLike(const QString& text)
{
    QString s = text;
    s.replace(QStringLiteral("\\"), QStringLiteral("\\\\"));
    s.replace(QStringLiteral("%"), QStringLiteral("\\%"));
    s.replace(QStringLiteral("_"), QStringLiteral("\\_"));
    s.replace(QStringLiteral("'"), QStringLiteral("''"));
    return s;
}

void DoctorModel::setKeywordFilter(const QString& keyword)
{
    const auto trimmed = keyword.trimmed();
    if (trimmed.isEmpty()) {
        setFilter({});
        select();
        return;
    }
    const auto like = QStringLiteral("%%%1%%").arg(escapeLike(trimmed));
    setFilter(QStringLiteral("(EMPLOYEENO LIKE '%1' ESCAPE '\\' OR NAME LIKE '%1' ESCAPE '\\')").arg(like));
    select();
}

