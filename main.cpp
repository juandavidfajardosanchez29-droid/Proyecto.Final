/**
 * @file main.cpp
 * @brief Punto de entrada del Gestor de Inventario.
 *
 * @mainpage Gestor de Inventario
 *
 * Aplicación de escritorio en C++/Qt para administrar un inventario
 * con base de datos SQLite.
 *
 * @section funciones Funcionalidades
 * - Registro, edición y eliminación de artículos
 * - Búsqueda y filtrado dinámico por nombre, ubicación y categoría
 * - Alerta visual de stock bajo
 * - Exportación de reportes a CSV
 *
 * @section clases Clases principales
 * - DatabaseManager: conexión y creación de tablas
 * - Articulo: representa un artículo del inventario
 * - ArticulosModel: modelo de la tabla, resalta el stock bajo
 * - ArticuloDialog: formulario para agregar y editar
 * - MainWindow: ventana principal
 */#include "mainwindow.h"
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