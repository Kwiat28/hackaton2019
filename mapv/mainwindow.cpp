#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    map = new QMapWidget(this);
        setCentralWidget(map);
        map->setFocus();
}

MainWindow::~MainWindow()
{
    delete ui;
}
