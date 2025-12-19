#pragma once

#include <QWidget>

class QLineEdit;
class QPushButton;
class QSqlQueryModel;
class QTableView;

class HistoryPage final : public QWidget
{
    Q_OBJECT

public:
    explicit HistoryPage(QWidget* parent = nullptr);
    ~HistoryPage() override;

    void refresh();

private:
    void onSearch();

    QLineEdit* m_keyword = nullptr;
    QPushButton* m_searchBtn = nullptr;
    QPushButton* m_refreshBtn = nullptr;
    QTableView* m_table = nullptr;
    QSqlQueryModel* m_model = nullptr;
};

