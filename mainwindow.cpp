#include "mainwindow.hpp"
#include "./ui_mainwindow.h"

#include "graphscene.hpp"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_scene{new GraphScene{this, new Graph{}}}
{
    ui->setupUi(this);
    ui->gView->setScene(m_scene);
    ui->gView->setGraphScene(m_scene);
    m_scene->setSceneRect(-5000, -5000, 10000, 10000);
    m_scene->setBackgroundBrush(Qt::white);

    connect(ui->gView, &PanningView::placementFinished, this, [this]() {
        ui->btnAddNode->setChecked(false);
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnAddNode_clicked(bool checked)
{
    ui->gView->setPlacementMode(checked);
}
