#include "graphscene.hpp"
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include "coordinateutils.hpp"
#include "edgegraphicsitem.hpp"
#include "nodegraphicsitem.hpp"
#include <qgraphicsview.h>

GraphScene::GraphScene(Graph *graph, QObject *parent)
    : QGraphicsScene(parent)
    , m_graph(graph)
{
    connect(m_graph, &Graph::nodeAdded, this, &GraphScene::handleNodeAdded);
    connect(m_graph, &Graph::nodeRemoved, this, &GraphScene::handleNodeRemoved);
    connect(m_graph, &Graph::nodeMoved, this, &GraphScene::handleNodeMoved);
    connect(m_graph, &Graph::nodeUpdated, this, &GraphScene::handleNodeUpdated);
    connect(m_graph, &Graph::edgeAdded, this, &GraphScene::handleEdgeAdded);
    connect(m_graph, &Graph::edgeRemoved, this, &GraphScene::handleEdgeRemoved);
    connect(m_graph, &Graph::edgeUpdated, this, &GraphScene::handleEdgeUpdated);
}

void GraphScene::setInteractionMode(InteractionMode mode)
{
    m_mode = mode;
    m_firstNodeForEdge.reset();
}

void GraphScene::handleNodeAdded(NodeId id)
{
    const Node *data = m_graph->findNode(id);
    if (!data)
        return;

    auto *item = new NodeGraphicsItem(id, *data);
    item->setPos(GraphUtils::mapGridToScenePos(data->pos));

    m_nodeItems[id] = item;
    addItem(item);
}

void GraphScene::handleEdgeAdded(EdgeId id)
{
    const Edge *data = m_graph->findEdge(id);
    if (!data)
        return;

    auto *item = new EdgeGraphicsItem(id, m_nodeItems[data->from], m_nodeItems[data->to]);
    item->updateFromModel(*data);

    m_edgeItems[id] = item;
    addItem(item);
}

void GraphScene::handleNodeRemoved(NodeId id)
{
    if (auto it = m_nodeItems.find(id); it != m_nodeItems.end()) {
        removeItem(it->second);
        delete it->second;
        m_nodeItems.erase(it);
    }
}

void GraphScene::handleEdgeRemoved(EdgeId id)
{
    if (auto it = m_edgeItems.find(id); it != m_edgeItems.end()) {
        removeItem(it->second);
        delete it->second;
        m_edgeItems.erase(it);
    }
}

void GraphScene::handleEdgeUpdated(EdgeId id)
{
    if (m_edgeItems.contains(id)) {
        const Edge *data = m_graph->findEdge(id);
        if (data) {
            m_edgeItems[id]->updateFromModel(*data);
        }
    }
}

void GraphScene::handleNodeMoved(NodeId id, QPointF gridPos)
{
    if (m_nodeItems.contains(id)) {
        m_nodeItems[id]->setPos(GraphUtils::mapGridToScenePos(gridPos));
    }
}

void GraphScene::handleNodeUpdated(NodeId id)
{
    if (m_nodeItems.contains(id)) {
        const Node *data = m_graph->findNode(id);
        if (data) {
            m_nodeItems[id]->updateFromModel(*data);
        }
    }
}

void GraphScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) {
        QGraphicsScene::mousePressEvent(event);
        return;
    }

    QPointF gridPos = GraphUtils::mapSceneToGridPos(event->scenePos());

    switch (m_mode) {
    case InteractionMode::PlaceNode: {
        if (!itemAt(event->scenePos(), QTransform{})) {
            m_graph->addNode(gridPos);
        }
        break;
    }
    case InteractionMode::PlaceEdge: {
        if (auto *item = dynamic_cast<NodeGraphicsItem *>(itemAt(event->scenePos(), QTransform()))) {
            if (!m_firstNodeForEdge) {
                m_firstNodeForEdge = item->id();
            } else {
                m_graph->addEdge(*m_firstNodeForEdge, item->id());
                m_firstNodeForEdge.reset();
            }
        } else {
            m_firstNodeForEdge.reset();
        }
        break;
    }
    case InteractionMode::Edit: {
        if (auto *item = itemAt(event->scenePos(), QTransform{})) {
            if (auto *nItem = dynamic_cast<NodeGraphicsItem *>(item)) {
                emit nodeEditRequested(nItem->id());
                return;
            } else if (auto *eItem = dynamic_cast<EdgeGraphicsItem *>(item)) {
                emit edgeEditRequested(eItem->id());
                return;
            }
        }
        break;
    }
    case InteractionMode::Delete: {
        if (auto *item = itemAt(event->scenePos(), QTransform{})) {
            if (auto *nItem = dynamic_cast<NodeGraphicsItem *>(item)) {
                int edgeCount = 0;
                for (const auto &[eId, edge] : m_graph->edges()) {
                    if (edge.from == nItem->id() || edge.to == nItem->id())
                        ++edgeCount;
                }

                if (edgeCount > 0) {
                    emit deletionRequested(nItem->id(), edgeCount);
                } else {
                    m_graph->removeNode(nItem->id());
                }
            } else if (auto *eItem = dynamic_cast<EdgeGraphicsItem *>(item)) {
                m_graph->removeEdge(eItem->id());
            }
        }
        break;
    }
    default:
        QGraphicsScene::mousePressEvent(event);
    }
}

void GraphScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (m_mode == InteractionMode::Select) {
        for (auto *item : selectedItems()) {
            if (auto *nItem = dynamic_cast<NodeGraphicsItem *>(item)) {
                m_graph->moveNode(nItem->id(), GraphUtils::mapSceneToGridPos(nItem->scenePos()));
            }
        }
    }
    QGraphicsScene::mouseReleaseEvent(event);
}

void GraphScene::drawBackground(QPainter *painter, const QRectF &rect)
{
    painter->fillRect(rect, Qt::white);

    int32_t left = qFloor(rect.left() / GraphUtils::gridSize) * GraphUtils::gridSize;
    int32_t top = qFloor(rect.top() / GraphUtils::gridSize) * GraphUtils::gridSize;

    // 100 is on stack but if you unzoom too much it moves to the heap
    QVarLengthArray<QLine, 100> lines;

    for (int32_t x = left; x <= rect.right(); x += GraphUtils::gridSize) {
        lines.append(QLine(x, (int) rect.top(), x, (int) rect.bottom()));
    }

    for (int32_t y = top; y <= rect.bottom(); y += GraphUtils::gridSize) {
        lines.append(QLine((int) rect.left(), y, (int) rect.right(), y));
    }

    painter->setPen(QPen(Qt::black, 0));
    painter->drawLines(lines.data(), lines.size());
}

void GraphScene::drawForeground(QPainter *painter, const QRectF &rect)
{
    painter->setPen(QPen(Qt::black));
    QFont font = painter->font();
    font.setPointSize(8);
    painter->setFont(font);

    int32_t leftStart = qFloor(rect.left() / GraphUtils::gridSize) * GraphUtils::gridSize;
    int32_t topStart = qFloor(rect.top() / GraphUtils::gridSize) * GraphUtils::gridSize;

    for (int32_t x = leftStart; x <= rect.right(); x += GraphUtils::gridSize) {
        painter->drawText(x + 2,
                          rect.bottom() - 15,
                          50,
                          20,
                          Qt::AlignLeft,
                          QString::number(x / GraphUtils::gridSize * 10));
    }

    for (int32_t y = topStart; y <= rect.bottom(); y += GraphUtils::gridSize) {
        painter->drawText(rect.right() - 40,
                          y + 2,
                          35,
                          20,
                          Qt::AlignRight,
                          QString::number(y / GraphUtils::gridSize * -10));
    }
}