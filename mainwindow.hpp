#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>

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
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btnAddNode_clicked(bool checked);

    void on_btnAddEdge_clicked(bool checked);

private:
    Ui::MainWindow *ui;
    GraphScene *m_scene;
};
#endif // MAINWINDOW_HPP
