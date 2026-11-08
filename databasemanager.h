#pragma once

#include <QString>
#include <QSqlDatabase>


class DatabaseManager
{
public:
    DatabaseManager();
    ~DatabaseManager();


    bool abrir(const QString &ruta);

private:
    bool crearTablas();
    QSqlDatabase m_db;
};