#include "mainwindow.hpp"
#include <QFutureWatcher>
#include <QMessageBox>
#include <QtConcurrent>
#include "SASolver.hpp"
#include "editedgedialog.hpp"
#include "editnodedialog.hpp"
#include "graphscene.hpp"
#include "optimizationdialog.hpp"
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
    OptimizationDialog dlg{this};
    if (dlg.exec() != QDialog::Accepted) {
        return;
    }

    SolverContext context{.params = dlg.params()};

    for (const auto &[id, node] : m_graph->nodes()) {
        if (node.type == NodeType::Customer) {
            context.customers.push_back(SolverNode{.id = id, .pos = node.pos});
        } else if (node.type == NodeType::PMedianCandidate) {
            context.candidates.push_back(SolverNode{.id = id, .pos = node.pos});
        }
    }

    for (const auto &[id, edge] : m_graph->edges()) {
        if (!edge.isEnabled) {
            continue;
        }
        context.adjList[edge.from].push_back(SolverEdge{.to = edge.to, .length = edge.length});
        context.adjList[edge.to].push_back(SolverEdge{.to = edge.from, .length = edge.length});
    }

    if (context.customers.empty() || context.candidates.empty()) {
        QMessageBox::warning(
            this,
            "Optimization Error",
            "The graph must contain at least one Customer and one P-Median Candidate.");
        return;
    }

    this->setEnabled(false);

    auto watcher = std::make_unique<QFutureWatcher<SolverResult>>();
    auto *watcherPtr = watcher.get();
    connect(watcherPtr, &QFutureWatcher<SolverResult>::finished, this, [this, watcherPtr]() {
        const SolverResult result = watcherPtr->result();

        for (const NodeId id : result.chosenCandidates) {
            m_graph->setNodeType(id, NodeType::ChosenMedian);
        }

        for (const auto &[customerId, medianId] : result.assignments) {
            m_graph->addSolutionEdge(customerId, medianId);
        }

        this->setEnabled(true);
        watcherPtr->deleteLater();
    });

    watcher.release()->setParent(this);
    watcherPtr->setFuture(QtConcurrent::run(
        [ctx = std::move(context)]() -> SolverResult { return SASolver::solve(ctx); }));
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

void MainWindow::on_btnDeleteSol_clicked()
{
    m_graph->removeSolution();
}

void MainWindow::on_btnClear_clicked()
{
    m_graph->clear();
}

void MainWindow::on_btnLoad_clicked()
{
    if (m_graph->load("graph.json")) {
        ui->statusbar->showMessage(tr("Loaded"));
    } else {
        ui->statusbar->showMessage(tr("Error loading a file"));
    }
}

void MainWindow::on_btnSave_clicked()
{
    if (m_graph->save("graph.json")) {
        ui->statusbar->showMessage(tr("Saved"));
    } else {
        ui->statusbar->showMessage(tr("Error saving a file"));
    }
}
void MainWindow::on_btnConnectivity_clicked()
{
    if (m_graph->isFullyConnected()) {
        QMessageBox::warning(this, "Connectivity Test", "The graph is fully connected!");
    } else {
        QMessageBox::warning(this, "Connectivity Test", "The graph is NOT fully connected!");
    }
}
