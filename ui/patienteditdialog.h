#pragma once

#include <QDialog>

#include "entities/patient.h"

class QComboBox;
class QDateEdit;
class QDoubleSpinBox;
class QLineEdit;
class QSpinBox;

class PatientEditDialog final : public QDialog
{
    Q_OBJECT

public:
    explicit PatientEditDialog(QWidget* parent = nullptr);

    void setPatient(const Patient& p);
    Patient patient() const;

private:
    QLineEdit* m_id = nullptr;
    QLineEdit* m_name = nullptr;
    QLineEdit* m_idCard = nullptr;
    QComboBox* m_sex = nullptr;
    QDateEdit* m_dob = nullptr;
    QDoubleSpinBox* m_height = nullptr;
    QDoubleSpinBox* m_weight = nullptr;
    QLineEdit* m_mobile = nullptr;
    QSpinBox* m_age = nullptr;
};

