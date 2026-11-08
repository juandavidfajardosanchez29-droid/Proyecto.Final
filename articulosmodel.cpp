#include "articulosmodel.h"


#include <QBrush>
#include <QColor>

ArticulosModel::ArticulosModel(QObject *parent)
    : QSqlRelationalTableModel(parent)
{
}

QVariant ArticulosModel::data(const QModelIndex &idx, int role) const
{
    if (idx.isValid() && role == Qt::BackgroundRole) {
        int cantidad = QSqlRelationalTableModel::data(
                           index(idx.row(), 3), Qt::DisplayRole).toInt();
        int minimo = QSqlRelationalTableModel::data(
                         index(idx.row(), 4), Qt::DisplayRole).toInt();

        if (cantidad <= minimo)
            return QBrush(QColor(170, 40, 40));
    }
    return QSqlRelationalTableModel::data(idx, role);
}