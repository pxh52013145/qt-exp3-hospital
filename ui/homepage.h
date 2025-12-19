#pragma once

#include <QWidget>

class QPushButton;

class HomePage final : public QWidget
{
    Q_OBJECT

public:
    explicit HomePage(QWidget* parent = nullptr);

signals:
    void openPatients();
    void openDoctors();
    void openDepartments();

private:
    QPushButton* m_patientBtn = nullptr;
    QPushButton* m_doctorBtn = nullptr;
    QPushButton* m_departmentBtn = nullptr;
};

