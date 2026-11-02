#include "articulo.h"

Articulo::Articulo(int id, const QString &nombre, int categoriaId,
                   int cantidad, int stockMinimo,
                   const QString &ubicacion, const QString &fecha)
    : m_id(id), m_nombre(nombre), m_categoriaId(categoriaId),
    m_cantidad(cantidad), m_stockMinimo(stockMinimo),
    m_ubicacion(ubicacion), m_fecha(fecha)
{
}

int Articulo::id() const { return m_id; }
QString Articulo::nombre() const { return m_nombre; }
int Articulo::categoriaId() const { return m_categoriaId; }
int Articulo::cantidad() const { return m_cantidad; }
int Articulo::stockMinimo() const { return m_stockMinimo; }
QString Articulo::ubicacion() const { return m_ubicacion; }
QString Articulo::fecha() const { return m_fecha; }

bool Articulo::stockBajo() const { return m_cantidad <= m_stockMinimo; }