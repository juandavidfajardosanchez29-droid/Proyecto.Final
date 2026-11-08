#pragma once

#include <QDialog>

class QLineEdit;
class QComboBox;
class QSpinBox;
class QDateEdit;

/**
 * @brief Formulario para agregar o editar un artículo.
 */
class ArticuloDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ArticuloDialog(QWidget *parent = nullptr);

    QString nombre() const;
    int categoriaId() const;
    int cantidad() const;
    int stockMinimo() const;
    QString ubicacion() const;
    QString fecha() const;

    /** @brief Llena el formulario con los datos de un artículo existente. */
    void setDatos(const QString &nombre, const QString &categoria,
                  int cantidad, int stockMinimo,
                  const QString &ubicacion, const QString &fecha);

    /** @brief Valida los datos antes de cerrar el diálogo. */
    void accept() override;

private:
    QLineEdit *m_nombre;
    QComboBox *m_categoria;
    QSpinBox *m_cantidad;
    QSpinBox *m_stockMinimo;
    QLineEdit *m_ubicacion;
    QDateEdit *m_fecha;
};