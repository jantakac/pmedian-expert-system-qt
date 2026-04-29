#include "graphscene.hpp"

#include <QPainter>
#include <QPointer>
#include <QVarLengthArray>
#include <iostream>

#include "coordinateutils.hpp"
#include "edgegraphicsitem.hpp"
#include "graph.hpp"
#include "nodegraphicsitem.hpp"

GraphScene::GraphScene(QObject *parent, Graph *backendGraph)
    : QGraphicsScene{parent}
    , m_previewNode{new NodeGraphicsItem{"", QPointF{0, 0}, 0, nodeSize, false}}
{
    if (backendGraph)
        setBackendGraph(backendGraph);
    m_previewNode->setCacheMode(QGraphicsItem::DeviceCoordinateCache);
    addItem(m_previewNode);
    m_previewNode->hide();
}

void GraphScene::setBackendGraph(Graph *backendGraph)
{
    if (m_backend == backendGraph)
        return;
    if (m_backend) {
        m_backend->disconnect(this);
        m_backend->deleteLater();
    }
    m_backend = backendGraph;
    m_backend->setParent(this);
}

void GraphScene::addNode(const QPointF &scenePos)
{
    uint32_t addedId = m_backend->addNode(GraphUtils::mapSceneToGridPos(scenePos)).id;
    NodeGraphicsItem *nodeG = new NodeGraphicsItem{QString::number(addedId),
                                                   scenePos,
                                                   addedId,
                                                   nodeSize,
                                                   true};
    m_nodeGItems.emplace(addedId, nodeG);
    addItem(nodeG);
    connect(nodeG, &NodeGraphicsItem::nodeSelected, this, &GraphScene::onNodeSelected);
    connect(nodeG, &NodeGraphicsItem::nodeMoveFinished, this, &GraphScene::handleNodeMoveFinished);
}

void GraphScene::addEdge(uint32_t nodeIdFrom, uint32_t nodeIdTo)
{
    if (m_backend->edgeByNodes(nodeIdFrom, nodeIdTo))
        return;
    NodeGraphicsItem *nodeGFrom = m_nodeGItems[nodeIdFrom];
    NodeGraphicsItem *nodeGTo = m_nodeGItems[nodeIdTo];
    uint32_t addedId = m_backend->addEdge(nodeIdFrom, nodeIdTo).id;
    EdgeGraphicsItem *edgeG = new EdgeGraphicsItem{QLineF{GraphUtils::mapGridToScenePos(
                                                              m_backend->nodeById(nodeIdFrom)->pos),
                                                          GraphUtils::mapGridToScenePos(
                                                              m_backend->nodeById(nodeIdTo)->pos)},
                                                   nodeGFrom,
                                                   nodeGTo,
                                                   addedId};
    m_edgeGItems.emplace(addedId, edgeG);
    addItem(edgeG);
    nodeGFrom->addConnectedEdge(edgeG);
    nodeGTo->addConnectedEdge(edgeG);
    connect(edgeG, &EdgeGraphicsItem::edgeSelected, this, &GraphScene::onEdgeSelected);
}

void GraphScene::updateNode(const Node &node)
{
    m_backend->editNode(node);
    if (m_nodeGItems.contains(node.id)) {
        m_nodeGItems[node.id]->updateVisuals(node);
    }
}

void GraphScene::updateEdge(const Edge &edge)
{
    m_backend->editEdge(edge);
}

const Node &GraphScene::backendNodeById(uint32_t id)
{
    return *m_backend->nodeById(id);
}

const Edge &GraphScene::backendEdgeById(uint32_t id)
{
    return *m_backend->edgeById(id);
}

void GraphScene::showPreviewNode()
{
    m_previewNode->show();
}

void GraphScene::hidePreviewNode()
{
    m_previewNode->hide();
}

void GraphScene::setPreviewNodePos(const QPointF &pos)
{
    m_previewNode->setPos(pos);
}

void GraphScene::onNodeSelected(uint32_t nodeId)
{
    std::cout << "ID: " << m_backend->nodeById(nodeId)->id << std::endl;
}

void GraphScene::onEdgeSelected(uint32_t edgeId)
{
    std::cout << "edge ID: " << m_backend->edgeById(edgeId)->id << std::endl;
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

void GraphScene::handleNodeMoveFinished(uint32_t nodeId, QPointF newScenePos)
{
    if (!m_backend)
        return;

    QPointF gridPos = GraphUtils::mapSceneToGridPos(newScenePos);

    const Node *existingNode = m_backend->nodeById(nodeId);
    if (existingNode) {
        Node updatedNode = *existingNode;
        updatedNode.pos = gridPos;
        m_backend->editNode(updatedNode);
    }
}
