#include "patientmodel.h"

#include <QSqlRecord>

PatientModel::PatientModel(QObject* parent)
    : QSqlTableModel(parent)
{
    setTable(QStringLiteral("Patient"));
    setEditStrategy(QSqlTableModel::OnFieldChange);
    select();

    setHeaderData(columnIndex(QStringLiteral("ID")), Qt::Horizontal, QStringLiteral("ID"));
    setHeaderData(columnIndex(QStringLiteral("ID_CARD")), Qt::Horizontal, QStringLiteral("身份证"));
    setHeaderData(columnIndex(QStringLiteral("NAME")), Qt::Horizontal, QStringLiteral("姓名"));
    setHeaderData(columnIndex(QStringLiteral("SEX")), Qt::Horizontal, QStringLiteral("性别"));
    setHeaderData(columnIndex(QStringLiteral("DOB")), Qt::Horizontal, QStringLiteral("出生日期"));
    setHeaderData(columnIndex(QStringLiteral("HEIGHT")), Qt::Horizontal, QStringLiteral("身高(cm)"));
    setHeaderData(columnIndex(QStringLiteral("WEIGHT")), Qt::Horizontal, QStringLiteral("体重(kg)"));
    setHeaderData(columnIndex(QStringLiteral("MOBILEPHONE")), Qt::Horizontal, QStringLiteral("手机号"));
    setHeaderData(columnIndex(QStringLiteral("AGE")), Qt::Horizontal, QStringLiteral("年龄"));
    setHeaderData(columnIndex(QStringLiteral("CREATEDTIMESTAMP")), Qt::Horizontal, QStringLiteral("创建时间"));
}

QString PatientModel::escapeLike(const QString& text)
{
    QString s = text;
    s.replace(QStringLiteral("\\"), QStringLiteral("\\\\"));
    s.replace(QStringLiteral("%"), QStringLiteral("\\%"));
    s.replace(QStringLiteral("_"), QStringLiteral("\\_"));
    s.replace(QStringLiteral("'"), QStringLiteral("''"));
    return s;
}

void PatientModel::setKeywordFilter(const QString& keyword)
{
    const auto trimmed = keyword.trimmed();
    if (trimmed.isEmpty()) {
        setFilter({});
        select();
        return;
    }

    const auto k = escapeLike(trimmed);
    const auto like = QStringLiteral("%%%1%%").arg(k);
    setFilter(QStringLiteral("(ID_CARD LIKE '%1' ESCAPE '\\' OR NAME LIKE '%1' ESCAPE '\\' OR MOBILEPHONE LIKE '%1' ESCAPE '\\')")
                  .arg(like));
    select();
}

int PatientModel::columnIndex(const QString& fieldName) const
{
    for (int i = 0; i < record().count(); ++i) {
        if (record().fieldName(i).compare(fieldName, Qt::CaseInsensitive) == 0) {
            return i;
        }
    }
    return -1;
}

int PatientModel::sexColumn() const { return columnIndex(QStringLiteral("SEX")); }
int PatientModel::dobColumn() const { return columnIndex(QStringLiteral("DOB")); }
int PatientModel::heightColumn() const { return columnIndex(QStringLiteral("HEIGHT")); }
int PatientModel::weightColumn() const { return columnIndex(QStringLiteral("WEIGHT")); }
int PatientModel::ageColumn() const { return columnIndex(QStringLiteral("AGE")); }

