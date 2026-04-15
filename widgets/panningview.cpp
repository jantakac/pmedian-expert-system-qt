#include "panningview.hpp"

#include <QMouseEvent>
#include <QOpenGLWidget>
#include <QScrollBar>

#include "../graphscene.hpp"

PanningView::PanningView(QWidget *parent)
    : QGraphicsView(parent)
{
    setCacheMode(QGraphicsView::CacheBackground);
    setViewport(new QOpenGLWidget{});
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setRenderHint(QPainter::Antialiasing);
    setMouseTracking(true);
    m_throttleTimer.start();
}

void PanningView::setNodePlacementMode(bool active)
{
    m_isPlacingNode = active;

    if (m_isPlacingNode) {
        m_graphScene->showPreviewNode();
        setCursor(Qt::CrossCursor);
    } else {
        m_graphScene->hidePreviewNode();
        setCursor(Qt::ArrowCursor);
    }
}

void PanningView::setEdgePlacementMode(bool active)
{
    m_isPlacingEdge = active;
}

void PanningView::setGraphScene(GraphScene *graphScene)
{
    m_graphScene = graphScene;
}

void PanningView::mousePressEvent(QMouseEvent *event)
{
    if (m_isPlacingNode && event->button() == Qt::LeftButton) {
        QPointF scenePos = mapToScene(event->position().toPoint());
        m_graphScene->addNode(scenePos);

        setNodePlacementMode(false);
        emit nodePlacementFinished();
        return;
    }

    if (event->button() == Qt::MiddleButton) {
        m_lastPanPoint = event->pos();
        setCursor(Qt::ClosedHandCursor);
    }
    QGraphicsView::mousePressEvent(event);
}

void PanningView::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MiddleButton) {
        if (!m_isPlacingNode)
            setCursor(Qt::ArrowCursor);
        else
            setCursor(Qt::CrossCursor);
    }
    QGraphicsView::mouseReleaseEvent(event);
}

void PanningView::mouseMoveEvent(QMouseEvent *event)
{
    if (m_throttleTimer.elapsed() > throttleThresholdMs) {
        m_throttleTimer.restart();
        if (m_isPlacingNode)
            m_graphScene->setPreviewNodePos(mapToScene(event->position().toPoint()));

        if (event->buttons() & Qt::MiddleButton) {
            QPoint eventPos = event->position().toPoint();
            QPoint delta = eventPos - m_lastPanPoint;
            m_lastPanPoint = eventPos;
            horizontalScrollBar()->setValue(horizontalScrollBar()->value() - delta.x());
            verticalScrollBar()->setValue(verticalScrollBar()->value() - delta.y());
        }
    }
    QGraphicsView::mouseMoveEvent(event);
}
