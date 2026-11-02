#include "mainwindow.h"
#include "databasemanager.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    DatabaseManager db;
    if (!db.abrir("inventario.db"))
        return 1;

    MainWindow w;
    w.show();
    return a.exec();
}