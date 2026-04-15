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
    NodeGraphicsItem *m_selectedNode = nullptr;
    QGraphicsEllipseItem *m_previewNode = nullptr;
    QElapsedTimer m_throttleTimer;
    bool m_isPlacingNode = false;
};

#endif // PANNINGVIEW_HPP
