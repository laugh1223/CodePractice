#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("openglDemo");
    setCentralWidget(ui->openGLWidget);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_actiondraw_triggered()
{
    ui->openGLWidget->draw(true);
}


void MainWindow::on_actionclear_triggered()
{
    ui->openGLWidget->draw(false);
}

void MainWindow::on_actionwireframe_triggered(bool checked)
{
    ui->openGLWidget->setWireframe(checked);
}

