#pragma once

#include <QDate>
#include <QString>

struct Patient
{
    QString id;
    QString idCard;
    QString name;
    int sex = 0; // 0女 1男
    QDate dob;
    double height = 0.0;
    double weight = 0.0;
    QString mobilePhone;
    int age = 0;
};

