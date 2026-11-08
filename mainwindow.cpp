#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "articulodialog.h"
#include "articulosmodel.h"
#include "articulo.h"

#include <QInputDialog>
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QVector>
#include <QLineEdit>
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

    m_modelo = new ArticulosModel(this);
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
    auto *btnExportar = new QPushButton("Exportar CSV", this);
    connect(btnExportar, &QPushButton::clicked, this, &MainWindow::exportarCSV);
    auto *btnCategoria = new QPushButton("Nueva categoría", this);
    connect(btnCategoria, &QPushButton::clicked, this, &MainWindow::agregarCategoria);


    auto *filaBotones = new QHBoxLayout;
    filaBotones->addWidget(btnAgregar);
    filaBotones->addWidget(btnEditar);
    filaBotones->addWidget(btnEliminar);
    filaBotones->addWidget(btnExportar);
    filaBotones->addWidget(btnCategoria);
    filaBotones->addStretch();




    m_buscador = new QLineEdit(this);
    m_buscador->setPlaceholderText("Buscar por nombre o ubicación...");
    m_buscador->setClearButtonEnabled(true);

    m_filtroCategoria = new QComboBox(this);
    m_filtroCategoria->addItem("Todas las categorías", 0);
    QSqlQuery qc("SELECT id, nombre FROM categorias ORDER BY nombre");
    while (qc.next())
        m_filtroCategoria->addItem(qc.value(1).toString(), qc.value(0));

    connect(m_buscador, &QLineEdit::textChanged, this, &MainWindow::aplicarFiltro);
    connect(m_filtroCategoria, &QComboBox::currentIndexChanged,
            this, &MainWindow::aplicarFiltro);

    auto *filaBusqueda = new QHBoxLayout;
    filaBusqueda->addWidget(m_buscador, 1);
    filaBusqueda->addWidget(m_filtroCategoria);

    auto *contenedor = new QWidget(this);
    auto *layout = new QVBoxLayout(contenedor);
    layout->addLayout(filaBotones);
    layout->addLayout(filaBusqueda);
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




void MainWindow::aplicarFiltro()
{
    QString texto = m_buscador->text().trimmed();
    texto.replace("'", "''");   // protege las comillas

    QStringList condiciones;

    if (!texto.isEmpty()) {
        condiciones << "(articulos.nombre LIKE '%" + texto + "%' "
                                                             "OR articulos.ubicacion LIKE '%" + texto + "%')";
    }

    int categoriaId = m_filtroCategoria->currentData().toInt();
    if (categoriaId > 0)
        condiciones << "articulos.categoria_id = " + QString::number(categoriaId);

    m_modelo->setFilter(condiciones.join(" AND "));
}




void MainWindow::exportarCSV()
{
    QString ruta = QFileDialog::getSaveFileName(
        this, "Guardar reporte", "inventario.csv", "Archivos CSV (*.csv)");
    if (ruta.isEmpty())
        return;

    // 1. Leer la base de datos y crear objetos Articulo
    QSqlQuery q("SELECT a.id, a.nombre, a.categoria_id, a.cantidad, "
                "a.stock_minimo, a.ubicacion, a.fecha_adquisicion, c.nombre "
                "FROM articulos a JOIN categorias c ON a.categoria_id = c.id "
                "ORDER BY a.nombre");

    QVector<Articulo> articulos;
    QStringList categorias;
    while (q.next()) {
        articulos.append(Articulo(q.value(0).toInt(), q.value(1).toString(),
                                  q.value(2).toInt(), q.value(3).toInt(),
                                  q.value(4).toInt(), q.value(5).toString(),
                                  q.value(6).toString()));
        categorias.append(q.value(7).toString());
    }


    QFile archivo(ruta);
    if (!archivo.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Error", "No se pudo crear el archivo.");
        return;
    }

    QTextStream out(&archivo);
    out.setEncoding(QStringConverter::Utf8);
    out.setGenerateByteOrderMark(true);   // para que Excel lea bien las tildes

    // Pone el texto entre comillas y duplica las comillas internas
    auto celda = [](const QString &texto) {
        QString t = texto;
        t.replace("\"", "\"\"");
        return "\"" + t + "\"";
    };

    // 3. Escribir encabezado y filas
    out << "Nombre;Categoria;Cantidad;Stock minimo;Ubicacion;Fecha;Estado\n";
    for (int i = 0; i < articulos.size(); ++i) {
        const Articulo &a = articulos.at(i);
        out << celda(a.nombre()) << ";"
            << celda(categorias.at(i)) << ";"
            << a.cantidad() << ";"
            << a.stockMinimo() << ";"
            << celda(a.ubicacion()) << ";"
            << celda(a.fecha()) << ";"
            << (a.stockBajo() ? "STOCK BAJO" : "OK") << "\n";
    }

    archivo.close();
    QMessageBox::information(this, "Reporte",
                             "Se exportaron " + QString::number(articulos.size())
                                 + " artículos.");
}



void MainWindow::agregarCategoria()
{
    bool ok = false;
    QString nombre = QInputDialog::getText(
                         this, "Nueva categoría", "Nombre de la categoría:",
                         QLineEdit::Normal, "", &ok).trimmed();
    if (!ok || nombre.isEmpty())
        return;

    QSqlQuery q;
    q.prepare("INSERT INTO categorias (nombre) VALUES (?)");
    q.addBindValue(nombre);

    if (!q.exec()) {
        QMessageBox::warning(this, "Error",
                             "No se pudo crear la categoría (¿ya existe?).");
        return;
    }

    // Agrega la categoría nueva a la lista del filtro
    m_filtroCategoria->addItem(nombre, q.lastInsertId());
    m_modelo->select();
}