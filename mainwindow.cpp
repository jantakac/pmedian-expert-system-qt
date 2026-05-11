#include "mainwindow.hpp"
#include <QFutureWatcher>
#include <QMessageBox>
#include <QtConcurrent>
#include "editedgedialog.hpp"
#include "editnodedialog.hpp"
#include "graph.hpp"
#include "graphscene.hpp"
#include "solvertypes.hpp"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_btns = {ui->btnSelectMode,
              ui->btnAddNodeMode,
              ui->btnAddEdgeMode,
              ui->btnEditMode,
              ui->btnDeleteMode};
    m_graph = std::make_unique<Graph>(this);
    m_scene = new GraphScene(m_graph.get(), this);
    m_scene->setSceneRect(-5000, -5000, 10000, 10000);
    ui->panningView->setScene(m_scene);
    setupConnections();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupConnections()
{
    connect(m_scene, &GraphScene::nodeEditRequested, this, [this](NodeId id) {
        const Node *node = m_graph->findNode(id);
        if (!node)
            return;

        EditNodeDialog dlg{*node, this};
        if (dlg.exec() == QDialog::Accepted) {
            m_graph->updateNode(dlg.getUpdatedNode());
        }
    });

    // Handle Edge Edit
    connect(m_scene, &GraphScene::edgeEditRequested, this, [this](EdgeId id) {
        const Edge *edge = m_graph->findEdge(id);
        if (!edge)
            return;

        EditEdgeDialog dlg{*edge, this};
        if (dlg.exec() == QDialog::Accepted) {
            m_graph->updateEdge(dlg.getUpdatedEdge());
        }
    });

    connect(m_scene, &GraphScene::deletionRequested, this, [this](NodeId id, int count) {
        auto result = QMessageBox::warning(this,
                                           "Confirm Deletion",
                                           QString("Node has %1 connected edges. Delete anyway?")
                                               .arg(count),
                                           QMessageBox::Yes | QMessageBox::No);

        if (result == QMessageBox::Yes) {
            m_graph->removeNode(id);
        }
    });
}

void MainWindow::on_btnSelectMode_clicked()
{
    m_scene->setInteractionMode(InteractionMode::Select);

    for (auto &btn : m_btns) {
        btn->setChecked(false);
    }
    ui->btnSelectMode->setChecked(true);
}

void MainWindow::on_btnAddNodeMode_clicked()
{
    m_scene->setInteractionMode(InteractionMode::PlaceNode);

    for (auto &btn : m_btns) {
        btn->setChecked(false);
    }
    ui->btnAddNodeMode->setChecked(true);
}

void MainWindow::on_btnAddEdgeMode_clicked()
{
    m_scene->setInteractionMode(InteractionMode::PlaceEdge);

    for (auto &btn : m_btns) {
        btn->setChecked(false);
    }
    ui->btnAddEdgeMode->setChecked(true);
}

void MainWindow::on_btnEditMode_clicked()
{
    m_scene->setInteractionMode(InteractionMode::Edit);

    for (auto &btn : m_btns) {
        btn->setChecked(false);
    }
    ui->btnEditMode->setChecked(true);
}

void MainWindow::on_btnDeleteMode_clicked()
{
    m_scene->setInteractionMode(InteractionMode::Delete);

    for (auto &btn : m_btns) {
        btn->setChecked(false);
    }
    ui->btnDeleteMode->setChecked(true);
}


void MainWindow::on_btnRunOptimization_clicked()
{
    std::vector<SolverNode> nodes;
    for (const auto &[id, node] : m_graph->nodes()) {
        nodes.emplace_back(id, node.pos);
    }

    this->setEnabled(false);

    auto solverTask = [nodes = std::move(nodes)]() -> SolverResult {
        std::vector<NodeId> results;
        if (!nodes.empty())
            results.push_back(nodes[0].id);

        QThread::msleep(2000); // Simulate heavy work
        return SolverResult{.chosenCandidates = results};
    };

    auto *watcher = new QFutureWatcher<SolverResult>(this);
    connect(watcher, &QFutureWatcher<SolverResult>::finished, this, [this, watcher]() {
        SolverResult result = watcher->result();
        for (NodeId id : result.chosenCandidates) {
            // Logic to update node type in m_graph (needs a method in Graph)
            // m_graph->setNodeType(id, NodeType::PMedianCandidate);
        }

        this->setEnabled(true);
        watcher->deleteLater();
    });

    watcher->setFuture(QtConcurrent::run(solverTask));
}

void MainWindow::handleOptimizationFinished()
{
    SolverResult result = m_solverWatcher.result();

    for (const auto &[id, node] : m_graph->nodes()) {
        if (node.type == NodeType::PMedianCandidate) {
            m_graph->setNodeType(id, NodeType::Customer);
        }
    }

    for (NodeId id : result.chosenCandidates) {
        m_graph->setNodeType(id, NodeType::PMedianCandidate);
    }

    setUiEnabled(true);

    if (result.chosenCandidates.empty()) {
        ui->statusbar->showMessage(tr("Optimization failed: No candidates found."), 5000);
    } else {
        ui->statusbar->showMessage(tr("Optimization successful: %1 nodes selected.")
                                       .arg(result.chosenCandidates.size()),
                                   5000);
    }
}

void MainWindow::setUiEnabled(bool enabled)
{
    ui->centralwidget->setEnabled(enabled);

    if (!enabled) {
        setCursor(Qt::WaitCursor);
    } else {
        unsetCursor();
    }
}
