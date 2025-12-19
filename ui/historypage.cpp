#include "historypage.h"

#include "db/dbmanager.h"

#include <QHeaderView>
#include <QLineEdit>
#include <QPushButton>
#include <QSqlQueryModel>
#include <QTableView>
#include <QVBoxLayout>

static QString escapeLike(const QString& text)
{
    QString s = text;
    s.replace(QStringLiteral("\\"), QStringLiteral("\\\\"));
    s.replace(QStringLiteral("%"), QStringLiteral("\\%"));
    s.replace(QStringLiteral("_"), QStringLiteral("\\_"));
    return s;
}

HistoryPage::HistoryPage(QWidget* parent)
    : QWidget(parent)
{
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(20, 20, 20, 20);
    root->setSpacing(12);

    auto* top = new QHBoxLayout();
    m_keyword = new QLineEdit(this);
    m_keyword->setPlaceholderText(QStringLiteral("输入关键字（事件）"));
    m_searchBtn = new QPushButton(QStringLiteral("查找"), this);
    m_refreshBtn = new QPushButton(QStringLiteral("刷新"), this);

    top->addWidget(m_keyword, 1);
    top->addWidget(m_searchBtn);
    top->addWidget(m_refreshBtn);
    root->addLayout(top);

    m_table = new QTableView(this);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setAlternatingRowColors(true);
    m_table->horizontalHeader()->setStretchLastSection(true);
    root->addWidget(m_table, 1);

    connect(m_searchBtn, &QPushButton::clicked, this, &HistoryPage::onSearch);
    connect(m_refreshBtn, &QPushButton::clicked, this, &HistoryPage::refresh);
    connect(m_keyword, &QLineEdit::returnPressed, this, &HistoryPage::onSearch);

    refresh();
}

HistoryPage::~HistoryPage()
{
    delete m_model;
}

void HistoryPage::refresh()
{
    delete m_model;
    m_model = nullptr;

    QString err;
    m_model = DbManager::instance().createQueryModel(
        QStringLiteral(
            "SELECT H.ID AS 'ID',"
            "       COALESCE(U.USERNAME,'') AS '用户名',"
            "       H.EVENT AS '事件',"
            "       H.TIMESTAMP AS '时间'"
            "  FROM History H"
            "  LEFT JOIN User U ON U.ID = H.USER_ID"
            " ORDER BY H.ID DESC;"),
        {},
        nullptr,
        &err);
    m_table->setModel(m_model);
}

void HistoryPage::onSearch()
{
    const auto k = m_keyword->text().trimmed();
    if (k.isEmpty()) {
        refresh();
        return;
    }
    delete m_model;
    m_model = nullptr;

    const auto like = QStringLiteral("%%%1%%").arg(escapeLike(k));
    QString err;
    m_model = DbManager::instance().createQueryModel(
        QStringLiteral(
            "SELECT H.ID AS 'ID',"
            "       COALESCE(U.USERNAME,'') AS '用户名',"
            "       H.EVENT AS '事件',"
            "       H.TIMESTAMP AS '时间'"
            "  FROM History H"
            "  LEFT JOIN User U ON U.ID = H.USER_ID"
            " WHERE H.EVENT LIKE ? ESCAPE '\\'"
            " ORDER BY H.ID DESC;"),
        {like},
        nullptr,
        &err);
    m_table->setModel(m_model);
}
