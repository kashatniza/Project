#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "scene.h"

#include <QGraphicsView>
#include <QColorDialog>
#include <QColor>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    mapScene(new Scene)
{
    ui->setupUi(this);

    QGraphicsView * pView = new QGraphicsView(this);
    pView->setScene(mapScene);
    setCentralWidget(pView);

    mapScene->connect(ui->actionPause,SIGNAL(triggered(bool)),SLOT(setPause(bool)));
}

MainWindow::~MainWindow()
{
    delete ui;
    delete mapScene;
}

void MainWindow::on_actionAddCar_triggered()
{
    QColor newCarColor = QColorDialog::getColor(QColor(Qt::black),this);
    if( newCarColor.isValid() ) {
        mapScene->enterAddCarWith(newCarColor);
    }
}

void MainWindow::on_actionRemoveCar_triggered()
{
    mapScene->enterDelCarMode();
}
