#include "panningview.hpp"

#include <QMouseEvent>
#include <QOpenGLWidget>
#include <QScrollBar>
#include <qgraphicsitem.h>

#include "../editedgedialog.hpp"
#include "../editnodedialog.hpp"
#include "../graphscene.hpp"
#include "../nodegraphicsitem.hpp"

PanningView::PanningView(QWidget *parent)
    : QGraphicsView(parent)
{
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
    if (m_isPlacingEdge) {
        setCursor(Qt::CrossCursor);
    } else {
        m_lastNodeIdClickedInEdgeMode = 0;
        setCursor(Qt::ArrowCursor);
    }
}

void PanningView::setGraphScene(GraphScene *graphScene)
{
    m_graphScene = graphScene;
}

void PanningView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if (m_isPlacingNode) {
            QPointF scenePos = mapToScene(event->position().toPoint());
            m_graphScene->addNode(scenePos);

            setNodePlacementMode(false);
            emit nodePlacementFinished();
            return;
        }
        if (m_isPlacingEdge) {
            edgePlacementHandler(event);
        }
    }

    if (event->button() == Qt::MiddleButton) {
        m_lastPanPoint = event->pos();
        setCursor(Qt::ClosedHandCursor);
    }

    if (event->button() == Qt::RightButton) {
        QGraphicsItem *item = itemAt(event->position().toPoint());

        if (item) {
            NodeGraphicsItem *nodeG = qgraphicsitem_cast<NodeGraphicsItem *>(item);

            if (!nodeG && item->parentItem()) {
                nodeG = qgraphicsitem_cast<NodeGraphicsItem *>(item->parentItem());
            }

            if (nodeG) {
                EditNodeDialog dialog{m_graphScene->backendNodeById(nodeG->backendNodeId()), this};

                if (dialog.exec() == QDialog::Accepted) {
                    Node updatedNode = dialog.editedNode();

                    m_graphScene->updateNode(updatedNode);

                    qDebug() << "Node" << nodeG->backendNodeId() << "updated.";
                }
            }
        }
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

void PanningView::edgePlacementHandler(QMouseEvent *event)
{
    uint32_t fromNodeId = m_lastNodeIdClickedInEdgeMode;

    setLastClickedItemInEdgeMode(itemAt(event->position().toPoint()));

    if (fromNodeId != 0 && m_lastNodeIdClickedInEdgeMode != 0
        && fromNodeId != m_lastNodeIdClickedInEdgeMode) {
        m_graphScene->addEdge(fromNodeId, m_lastNodeIdClickedInEdgeMode);
    }
}

void PanningView::setLastClickedItemInEdgeMode(QGraphicsItem *clickedItem)
{
    NodeGraphicsItem *nodeG = nullptr;
    while (clickedItem) {
        nodeG = qgraphicsitem_cast<NodeGraphicsItem *>(clickedItem);
        if (nodeG)
            break;
        clickedItem = clickedItem->parentItem();
    }

    if (nodeG) {
        m_lastNodeIdClickedInEdgeMode = nodeG->backendNodeId();
    } else {
        m_lastNodeIdClickedInEdgeMode = 0;
    }
}
