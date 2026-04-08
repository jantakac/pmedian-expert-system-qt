#include "mainwindow.hpp"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_scene{new GraphScene{this, new Graph{}}}
{
    ui->setupUi(this);
    ui->gView->setViewport(new QOpenGLWidget{});
    ui->gView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    ui->gView->setRenderHint(QPainter::Antialiasing);
    ui->gView->setScene(m_scene);
    m_scene->setSceneRect(-5000, -5000, 10000, 10000);
    m_scene->setBackgroundBrush(Qt::white);
}

MainWindow::~MainWindow()
{
    delete ui;
}
