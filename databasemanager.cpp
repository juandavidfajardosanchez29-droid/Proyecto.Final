#include "databasemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

DatabaseManager::DatabaseManager()
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
}

DatabaseManager::~DatabaseManager()
{
    if (m_db.isOpen())
        m_db.close();
}

bool DatabaseManager::abrir(const QString &ruta)
{
    m_db.setDatabaseName(ruta);
    if (!m_db.open()) {
        qDebug() << "Error al abrir BD:" << m_db.lastError().text();
        return false;
    }
    return crearTablas();
}

bool DatabaseManager::crearTablas()
{
    QSqlQuery q;
    if (!q.exec("PRAGMA foreign_keys = ON"))
        return false;

    bool ok = q.exec(
        "CREATE TABLE IF NOT EXISTS categorias ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "nombre TEXT NOT NULL UNIQUE)");

    ok = ok && q.exec(
             "CREATE TABLE IF NOT EXISTS articulos ("
             "id INTEGER PRIMARY KEY AUTOINCREMENT,"
             "nombre TEXT NOT NULL,"
             "categoria_id INTEGER NOT NULL,"
             "cantidad INTEGER NOT NULL DEFAULT 0,"
             "stock_minimo INTEGER NOT NULL DEFAULT 5,"
             "ubicacion TEXT,"
             "fecha_adquisicion TEXT,"
             "FOREIGN KEY (categoria_id) REFERENCES categorias(id))");

    if (!ok)
        qDebug() << "Error creando tablas:" << q.lastError().text();
    return ok;
}