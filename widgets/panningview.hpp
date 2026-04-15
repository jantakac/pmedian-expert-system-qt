#ifndef PANNINGVIEW_HPP
#define PANNINGVIEW_HPP

#include <QElapsedTimer>
#include <QGraphicsView>

class QMouseEvent;
class GraphScene;
class NodeGraphicsItem;

class PanningView : public QGraphicsView
{
    Q_OBJECT
public:
    static constexpr qint64 throttleThresholdMs = 10;

    explicit PanningView(QWidget *parent = nullptr);
    void setNodePlacementMode(bool active);
    void setEdgePlacementMode(bool active);
    void setGraphScene(GraphScene *graphScene);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

signals:
    void nodePlacementFinished();
    void edgePlacementFinished();

private:
    QPoint m_lastPanPoint = QPoint{};
    GraphScene *m_graphScene = nullptr;
    QElapsedTimer m_throttleTimer;
    bool m_isPlacingNode = false;
    bool m_isPlacingEdge = false;
};

#endif // PANNINGVIEW_HPP
