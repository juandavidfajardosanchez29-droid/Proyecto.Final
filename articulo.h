#pragma once

#include <QString>

/**
 * @brief Representa un artículo del inventario.
 */
class Articulo
{
public:
    Articulo(int id, const QString &nombre, int categoriaId,
             int cantidad, int stockMinimo,
             const QString &ubicacion, const QString &fecha);

    int id() const;
    QString nombre() const;
    int categoriaId() const;
    int cantidad() const;
    int stockMinimo() const;
    QString ubicacion() const;
    QString fecha() const;

    /** @brief Indica si la cantidad está por debajo del mínimo. */
    bool stockBajo() const;

private:
    int m_id;
    QString m_nombre;
    int m_categoriaId;
    int m_cantidad;
    int m_stockMinimo;
    QString m_ubicacion;
    QString m_fecha;
};