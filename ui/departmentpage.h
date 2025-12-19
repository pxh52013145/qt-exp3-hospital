#pragma once

#include <QWidget>

class DepartmentModel;
class QLineEdit;
class QPushButton;
class QTableView;

class DepartmentPage final : public QWidget
{
    Q_OBJECT

public:
    explicit DepartmentPage(QWidget* parent = nullptr);
    void setCurrentUserId(const QString& userId);

private:
    void onSearch();
    void onAdd();
    void onEdit();
    void onDelete();
    int selectedRow() const;

    QString m_userId;
    DepartmentModel* m_model = nullptr;
    QLineEdit* m_keyword = nullptr;
    QPushButton* m_searchBtn = nullptr;
    QPushButton* m_addBtn = nullptr;
    QPushButton* m_deleteBtn = nullptr;
    QPushButton* m_editBtn = nullptr;
    QTableView* m_table = nullptr;
};

