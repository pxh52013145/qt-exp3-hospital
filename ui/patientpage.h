#pragma once

#include <QWidget>

class PatientModel;
class QLineEdit;
class QPushButton;
class QTableView;

class PatientPage final : public QWidget
{
    Q_OBJECT

public:
    explicit PatientPage(QWidget* parent = nullptr);

    void setCurrentUserId(const QString& userId);

private:
    void onSearch();
    void onAdd();
    void onEdit();
    void onDelete();

    int selectedRow() const;

    QString m_userId;
    PatientModel* m_model = nullptr;

    QLineEdit* m_keyword = nullptr;
    QPushButton* m_searchBtn = nullptr;
    QPushButton* m_addBtn = nullptr;
    QPushButton* m_deleteBtn = nullptr;
    QPushButton* m_editBtn = nullptr;
    QTableView* m_table = nullptr;
};

