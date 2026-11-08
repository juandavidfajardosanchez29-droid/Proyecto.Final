#pragma once

#include <QSqlRelationalTableModel>

class ArticulosModel : public QSqlRelationalTableModel
{
    Q_OBJECT

public:
    explicit ArticulosModel(QObject *parent = nullptr);

    QVariant data(const QModelIndex &idx, int role = Qt::DisplayRole) const override;
};