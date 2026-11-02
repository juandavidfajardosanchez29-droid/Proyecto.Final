#pragma once

#include <QString>
#include <QSqlDatabase>

/**
 * @brief Gestiona la conexión y creación de tablas de SQLite.
 */
class DatabaseManager
{
public:
    DatabaseManager();
    ~DatabaseManager();

    /** @brief Abre la base de datos y crea las tablas si no existen. */
    bool abrir(const QString &ruta);

private:
    bool crearTablas();
    QSqlDatabase m_db;
};