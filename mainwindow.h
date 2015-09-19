#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class Scene;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionAddCar_triggered();

    void on_actionRemoveCar_triggered();

private:
    Ui::MainWindow *ui;
    Scene * mapScene;
};

#endif // MAINWINDOW_H
