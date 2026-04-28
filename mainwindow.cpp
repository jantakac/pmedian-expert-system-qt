#include "mainwindow.hpp"
#include "./ui_mainwindow.h"

#include "graph.hpp"
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

    connect(ui->gView, &PanningView::nodePlacementFinished, this, [this]() {
        ui->btnAddNode->setChecked(false);
    });
    connect(ui->gView, &PanningView::edgePlacementFinished, this, [this]() {
        ui->btnAddEdge->setChecked(false);
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnAddNode_clicked(bool checked)
{
    ui->btnAddEdge->setChecked(false);
    ui->gView->setEdgePlacementMode(false);
    ui->gView->setNodePlacementMode(checked);
}

void MainWindow::on_btnAddEdge_clicked(bool checked)
{
    ui->btnAddNode->setChecked(false);
    ui->gView->setNodePlacementMode(false);
    ui->gView->setEdgePlacementMode(checked);
}
