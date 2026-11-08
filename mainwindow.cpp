#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "articulodialog.h"

#include <QSqlError>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QPushButton>
#include <QSqlQuery>
#include <QSqlRelationalDelegate>
#include <QSqlRelationalTableModel>
#include <QTableView>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Gestor de Inventario");
    resize(900, 500);

    cargarDatosPrueba();

    m_modelo = new QSqlRelationalTableModel(this);
    m_modelo->setTable("articulos");
    // Columna 2 (categoria_id) se muestra con el nombre de la categoría
    m_modelo->setRelation(2, QSqlRelation("categorias", "id", "nombre"));
    m_modelo->setEditStrategy(QSqlTableModel::OnFieldChange);
    m_modelo->setHeaderData(1, Qt::Horizontal, "Nombre");
    m_modelo->setHeaderData(2, Qt::Horizontal, "Categoría");
    m_modelo->setHeaderData(3, Qt::Horizontal, "Cantidad");
    m_modelo->setHeaderData(4, Qt::Horizontal, "Stock mínimo");
    m_modelo->setHeaderData(5, Qt::Horizontal, "Ubicación");
    m_modelo->setHeaderData(6, Qt::Horizontal, "Fecha adquisición");
    m_modelo->select();

    m_tabla = new QTableView(this);
    m_tabla->setModel(m_modelo);
    m_tabla->setItemDelegate(new QSqlRelationalDelegate(m_tabla));
    m_tabla->hideColumn(0); // oculta el id
    m_tabla->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_tabla->setSelectionBehavior(QAbstractItemView::SelectRows);
    auto *btnAgregar = new QPushButton("Agregar", this);
    auto *btnEditar = new QPushButton("Editar", this);
    auto *btnEliminar = new QPushButton("Eliminar", this);
    connect(btnAgregar, &QPushButton::clicked, this, &MainWindow::agregarArticulo);
    connect(btnEditar, &QPushButton::clicked, this, &MainWindow::editarArticulo);
    connect(btnEliminar, &QPushButton::clicked, this, &MainWindow::eliminarArticulo);

    auto *filaBotones = new QHBoxLayout;
    filaBotones->addWidget(btnAgregar);
    filaBotones->addWidget(btnEditar);
    filaBotones->addWidget(btnEliminar);
    filaBotones->addStretch();

    auto *contenedor = new QWidget(this);
    auto *layout = new QVBoxLayout(contenedor);
    layout->addLayout(filaBotones);
    layout->addWidget(m_tabla);
    setCentralWidget(contenedor);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::cargarDatosPrueba()
{
    QSqlQuery q;
    q.exec("SELECT COUNT(*) FROM categorias");
    if (q.next() && q.value(0).toInt() > 0)
        return;

    q.exec("INSERT INTO categorias (nombre) VALUES ('Resistencias')");
    q.exec("INSERT INTO categorias (nombre) VALUES ('Capacitores')");
    q.exec("INSERT INTO categorias (nombre) VALUES ('Microcontroladores')");

    q.exec("INSERT INTO articulos (nombre, categoria_id, cantidad, stock_minimo, ubicacion, fecha_adquisicion) "
           "VALUES ('Resistencia 10k', 1, 100, 20, 'Cajón A1', '2026-10-01')");
    q.exec("INSERT INTO articulos (nombre, categoria_id, cantidad, stock_minimo, ubicacion, fecha_adquisicion) "
           "VALUES ('Capacitor 100nF', 2, 8, 15, 'Cajón B2', '2026-10-05')");
    q.exec("INSERT INTO articulos (nombre, categoria_id, cantidad, stock_minimo, ubicacion, fecha_adquisicion) "
           "VALUES ('ESP32', 3, 4, 2, 'Estante C', '2026-10-10')");

}
void MainWindow::agregarArticulo()
{
    ArticuloDialog dlg(this);
    if (dlg.exec() != QDialog::Accepted)
        return;

    QSqlQuery q;
    q.prepare("INSERT INTO articulos (nombre, categoria_id, cantidad, "
              "stock_minimo, ubicacion, fecha_adquisicion) "
              "VALUES (?, ?, ?, ?, ?, ?)");
    q.addBindValue(dlg.nombre());
    q.addBindValue(dlg.categoriaId());
    q.addBindValue(dlg.cantidad());
    q.addBindValue(dlg.stockMinimo());
    q.addBindValue(dlg.ubicacion());
    q.addBindValue(dlg.fecha());

    if (!q.exec())
        QMessageBox::warning(this, "Error", q.lastError().text());
    m_modelo->select();
}

void MainWindow::editarArticulo()
{
    int fila = m_tabla->currentIndex().row();
    if (fila < 0) {
        QMessageBox::information(this, "Editar", "Selecciona un artículo primero.");
        return;
    }

    int id = m_modelo->data(m_modelo->index(fila, 0)).toInt();

    ArticuloDialog dlg(this);
    dlg.setDatos(m_modelo->data(m_modelo->index(fila, 1)).toString(),
                 m_modelo->data(m_modelo->index(fila, 2)).toString(),
                 m_modelo->data(m_modelo->index(fila, 3)).toInt(),
                 m_modelo->data(m_modelo->index(fila, 4)).toInt(),
                 m_modelo->data(m_modelo->index(fila, 5)).toString(),
                 m_modelo->data(m_modelo->index(fila, 6)).toString());
    if (dlg.exec() != QDialog::Accepted)
        return;

    QSqlQuery q;
    q.prepare("UPDATE articulos SET nombre=?, categoria_id=?, cantidad=?, "
              "stock_minimo=?, ubicacion=?, fecha_adquisicion=? WHERE id=?");
    q.addBindValue(dlg.nombre());
    q.addBindValue(dlg.categoriaId());
    q.addBindValue(dlg.cantidad());
    q.addBindValue(dlg.stockMinimo());
    q.addBindValue(dlg.ubicacion());
    q.addBindValue(dlg.fecha());
    q.addBindValue(id);

    if (!q.exec())
        QMessageBox::warning(this, "Error", q.lastError().text());
    m_modelo->select();
}

void MainWindow::eliminarArticulo()
{
    int fila = m_tabla->currentIndex().row();
    if (fila < 0) {
        QMessageBox::information(this, "Eliminar", "Selecciona un artículo primero.");
        return;
    }

    QString nombre = m_modelo->data(m_modelo->index(fila, 1)).toString();
    auto respuesta = QMessageBox::question(
        this, "Eliminar", "¿Eliminar \"" + nombre + "\"?");
    if (respuesta != QMessageBox::Yes)
        return;

    int id = m_modelo->data(m_modelo->index(fila, 0)).toInt();
    QSqlQuery q;
    q.prepare("DELETE FROM articulos WHERE id=?");
    q.addBindValue(id);
    q.exec();
    m_modelo->select();
}