#include "articulodialog.h"

#include <QComboBox>
#include <QDateEdit>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QSpinBox>
#include <QSqlQuery>
#include <QVBoxLayout>

ArticuloDialog::ArticuloDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Artículo");

    m_nombre = new QLineEdit(this);
    m_categoria = new QComboBox(this);
    m_cantidad = new QSpinBox(this);
    m_cantidad->setRange(0, 1000000);
    m_stockMinimo = new QSpinBox(this);
    m_stockMinimo->setRange(0, 1000000);
    m_stockMinimo->setValue(5);
    m_ubicacion = new QLineEdit(this);
    m_fecha = new QDateEdit(QDate::currentDate(), this);
    m_fecha->setCalendarPopup(true);
    m_fecha->setDisplayFormat("yyyy-MM-dd");

    // Carga las categorías desde la base de datos
    QSqlQuery q("SELECT id, nombre FROM categorias ORDER BY nombre");
    while (q.next())
        m_categoria->addItem(q.value(1).toString(), q.value(0));

    auto *form = new QFormLayout;
    form->addRow("Nombre:", m_nombre);
    form->addRow("Categoría:", m_categoria);
    form->addRow("Cantidad:", m_cantidad);
    form->addRow("Stock mínimo:", m_stockMinimo);
    form->addRow("Ubicación:", m_ubicacion);
    form->addRow("Fecha adquisición:", m_fecha);

    auto *botones = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(botones, &QDialogButtonBox::accepted, this, &ArticuloDialog::accept);
    connect(botones, &QDialogButtonBox::rejected, this, &QDialog::reject);

    auto *layout = new QVBoxLayout(this);
    layout->addLayout(form);
    layout->addWidget(botones);
}

QString ArticuloDialog::nombre() const { return m_nombre->text().trimmed(); }
int ArticuloDialog::categoriaId() const { return m_categoria->currentData().toInt(); }
int ArticuloDialog::cantidad() const { return m_cantidad->value(); }
int ArticuloDialog::stockMinimo() const { return m_stockMinimo->value(); }
QString ArticuloDialog::ubicacion() const { return m_ubicacion->text().trimmed(); }
QString ArticuloDialog::fecha() const { return m_fecha->date().toString("yyyy-MM-dd"); }

void ArticuloDialog::setDatos(const QString &nombre, const QString &categoria,
                              int cantidad, int stockMinimo,
                              const QString &ubicacion, const QString &fecha)
{
    m_nombre->setText(nombre);
    m_categoria->setCurrentIndex(m_categoria->findText(categoria));
    m_cantidad->setValue(cantidad);
    m_stockMinimo->setValue(stockMinimo);
    m_ubicacion->setText(ubicacion);
    m_fecha->setDate(QDate::fromString(fecha, "yyyy-MM-dd"));
}

void ArticuloDialog::accept()
{
    if (nombre().isEmpty()) {
        QMessageBox::warning(this, "Dato faltante", "El nombre es obligatorio.");
        return;
    }
    if (m_categoria->count() == 0) {
        QMessageBox::warning(this, "Sin categorías", "No hay categorías registradas.");
        return;
    }
    QDialog::accept();
}