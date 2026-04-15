#include "panningview.hpp"

#include <QGraphicsEllipseItem>
#include <QMouseEvent>
#include <QScrollBar>

#include "../graphscene.hpp"
#include "../nodegraphicsitem.hpp"

PanningView::PanningView(QWidget *parent)
    : QGraphicsView(parent)
{
    setMouseTracking(true);
}

void PanningView::setPlacementMode(bool active)
{
    m_isPlacing = active;
    if (m_isPlacing)
        setCursor(Qt::CrossCursor);
    else
        setCursor(Qt::ArrowCursor);
}

void PanningView::setGraphScene(GraphScene *graphScene)
{
    m_graphScene = graphScene;
}

void PanningView::mousePressEvent(QMouseEvent *event)
{
    if (m_isPlacing && event->button() == Qt::LeftButton) {
        QPointF pos = mapToScene(event->position().toPoint());
        m_graphScene->addNode(pos);

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
        if (!m_isPlacing)
            setCursor(Qt::ArrowCursor);
        else
            setCursor(Qt::CrossCursor);
    }
    QGraphicsView::mouseReleaseEvent(event);
}

void PanningView::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::MiddleButton) {
        QPoint delta = event->pos() - m_lastPanPoint;
        m_lastPanPoint = event->pos();
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - delta.x());
        verticalScrollBar()->setValue(verticalScrollBar()->value() - delta.y());
    }

    QGraphicsView::mouseMoveEvent(event);
}
