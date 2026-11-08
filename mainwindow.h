#pragma once

#include <QMainWindow>

class QTableView;
class QSqlRelationalTableModel;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void agregarArticulo();
    void editarArticulo();
    void eliminarArticulo();

private:
    void cargarDatosPrueba();

    Ui::MainWindow *ui;
    QSqlRelationalTableModel *m_modelo;
    QTableView *m_tabla;


};
