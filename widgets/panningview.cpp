#include "panningview.hpp"
#include <QMouseEvent>
#include <QOpenGLWidget>
#include <QScrollBar>
#include <QWheelEvent>

PanningView::PanningView(QWidget *parent)
    : QGraphicsView(parent)
{
    setViewport(new QOpenGLWidget{});
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setMouseTracking(true);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setDragMode(QGraphicsView::NoDrag);
    // setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing
    //                | QPainter::SmoothPixmapTransform);
}

void PanningView::wheelEvent(QWheelEvent *event)
{
    const float factor = (event->angleDelta().y() > 0) ? zoomFactor : 1.0f / zoomFactor;
    const auto currentScale = transform().m11();
    if ((factor > 1.0f && currentScale < 10.0f) || (factor < 1.0f && currentScale > 0.1f)) {
        scale(factor, factor);
    }
}

void PanningView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MiddleButton) {
        m_isPanning = true;
        m_lastMousePos = event->pos();
        setCursor(Qt::ClosedHandCursor);
        event->accept();
        return;
    }
    QGraphicsView::mousePressEvent(event);
}

void PanningView::mouseMoveEvent(QMouseEvent *event)
{
    if (m_isPanning) {
        const QPoint delta = event->pos() - m_lastMousePos;
        m_lastMousePos = event->pos();

        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - delta.x());
        verticalScrollBar()->setValue(verticalScrollBar()->value() - delta.y());

        event->accept();
        return;
    }

    QGraphicsView::mouseMoveEvent(event);
}

void PanningView::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MiddleButton) {
        m_isPanning = false;
        unsetCursor();
        event->accept();
        return;
    }
    QGraphicsView::mouseReleaseEvent(event);
}