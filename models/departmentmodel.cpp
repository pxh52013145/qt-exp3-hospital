#include "departmentmodel.h"

DepartmentModel::DepartmentModel(QObject* parent)
    : QSqlTableModel(parent)
{
    setTable(QStringLiteral("Department"));
    setEditStrategy(QSqlTableModel::OnFieldChange);
    select();

    setHeaderData(0, Qt::Horizontal, QStringLiteral("ID"));
    setHeaderData(1, Qt::Horizontal, QStringLiteral("科室名称"));
}

QString DepartmentModel::escapeLike(const QString& text)
{
    QString s = text;
    s.replace(QStringLiteral("\\"), QStringLiteral("\\\\"));
    s.replace(QStringLiteral("%"), QStringLiteral("\\%"));
    s.replace(QStringLiteral("_"), QStringLiteral("\\_"));
    s.replace(QStringLiteral("'"), QStringLiteral("''"));
    return s;
}

void DepartmentModel::setKeywordFilter(const QString& keyword)
{
    const auto trimmed = keyword.trimmed();
    if (trimmed.isEmpty()) {
        setFilter({});
        select();
        return;
    }
    const auto like = QStringLiteral("%%%1%%").arg(escapeLike(trimmed));
    setFilter(QStringLiteral("NAME LIKE '%1' ESCAPE '\\'").arg(like));
    select();
}

