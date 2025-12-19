#pragma once

#include <QDialog>

class QComboBox;
class QLineEdit;

class DoctorEditDialog final : public QDialog
{
    Q_OBJECT

public:
    explicit DoctorEditDialog(QWidget* parent = nullptr);

    void setDoctor(const QString& id,
                   const QString& employeeNo,
                   const QString& name,
                   const QString& departmentId);

    QString id() const;
    QString employeeNo() const;
    QString name() const;
    QString departmentId() const;

private:
    void reloadDepartments();

    QLineEdit* m_id = nullptr;
    QLineEdit* m_employeeNo = nullptr;
    QLineEdit* m_name = nullptr;
    QComboBox* m_department = nullptr;
};

