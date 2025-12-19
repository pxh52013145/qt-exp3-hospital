#pragma once

#include <QDialog>

class QLineEdit;

class DepartmentEditDialog final : public QDialog
{
    Q_OBJECT

public:
    explicit DepartmentEditDialog(QWidget* parent = nullptr);

    void setDepartment(const QString& id, const QString& name);
    QString id() const;
    QString name() const;

private:
    QLineEdit* m_id = nullptr;
    QLineEdit* m_name = nullptr;
};

