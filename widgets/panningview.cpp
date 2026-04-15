#include "panningview.hpp"

#include <QMouseEvent>
#include <QOpenGLWidget>
#include <QScrollBar>

#include "../graphscene.hpp"
#include "../nodegraphicsitem.hpp"

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

void PanningView::setPlacementMode(bool active)
{
    m_isPlacingNode = active;

    if (m_isPlacingNode) {
        if (!m_previewNode) {
            m_previewNode = new NodeGraphicsItem{" ", QPointF{0, 0}, GraphScene::nodeSize};
            m_previewNode->setCacheMode(QGraphicsItem::DeviceCoordinateCache);
            scene()->addItem(m_previewNode);
        }
        m_previewNode->show();
        setCursor(Qt::CrossCursor);
    } else {
        if (m_previewNode) {
            m_previewNode->hide();
        }
        setCursor(Qt::ArrowCursor);
    }
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

        setPlacementMode(false);
        emit placementFinished();
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
        if (m_isPlacingNode && m_previewNode) {
            QPointF scenePos = mapToScene(event->position().toPoint());
            m_previewNode->setPos(scenePos.x(), scenePos.y());
        }

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
