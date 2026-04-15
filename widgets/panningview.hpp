#ifndef PANNINGVIEW_HPP
#define PANNINGVIEW_HPP

#include <QGraphicsView>

class QMouseEvent;
class GraphScene;

class PanningView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit PanningView(QWidget *parent = nullptr);
    void setPlacementMode(bool active);
    void setGraphScene(GraphScene *graphScene);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

signals:
    void placementFinished();

private:
    QPoint m_lastPanPoint = QPoint{};
    GraphScene *m_graphScene = nullptr;
    bool m_isPlacing = false;
};

#endif // PANNINGVIEW_HPP
