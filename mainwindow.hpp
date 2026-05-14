#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QFutureWatcher>
#include <QMainWindow>
#include "Graph.hpp"
#include "SASolver.hpp"
#include <memory>

class QPushButton;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class GraphScene;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void on_btnSelectMode_clicked();
    void on_btnAddNodeMode_clicked();
    void on_btnAddEdgeMode_clicked();
    void on_btnDeleteMode_clicked();

    void on_btnRunOptimization_clicked();
    void handleOptimizationFinished();

    void on_btnEditMode_clicked();

    void on_btnDeleteSol_clicked();

private:
    Ui::MainWindow *ui;

    std::unique_ptr<Graph> m_graph;
    GraphScene *m_scene;
    std::vector<QPushButton *> m_btns;

    QFutureWatcher<SolverResult> m_solverWatcher;

    void setupConnections();
    void setUiEnabled(bool enabled);
};

#endif // MAINWINDOW_HPP