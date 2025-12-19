#include "mainwindow.h"

#include <QApplication>
#include <QMessageBox>

#include "db/dbmanager.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QString err;
    if (!DbManager::instance().open(&err)) {
        QMessageBox::critical(nullptr, QStringLiteral("数据库错误"), err);
        return 1;
    }

    MainWindow w;
    w.show();
    return a.exec();
}
