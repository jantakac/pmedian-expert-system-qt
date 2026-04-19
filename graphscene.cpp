#include "graphscene.hpp"

#include <QPainter>
#include <QPointer>
#include <QVarLengthArray>
#include <iostream>

#include "edgegraphicsitem.hpp"
#include "graph.hpp"
#include "nodegraphicsitem.hpp"

GraphScene::GraphScene(QObject *parent, Graph *backendGraph)
    : QGraphicsScene{parent}
{
    if (backendGraph) {
        setBackendGraph(backendGraph);
    }
    m_previewNode->setCacheMode(QGraphicsItem::DeviceCoordinateCache);
    addItem(m_previewNode);
    m_previewNode->hide();
}

Graph &GraphScene::backend()
{
    return *m_backend;
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
    uint32_t addedId = m_backend->addNode(mapSceneToGridPos(scenePos)).id;
    NodeGraphicsItem *nodeG = new NodeGraphicsItem{QString::number(addedId),
                                                   scenePos,
                                                   addedId,
                                                   nodeSize,
                                                   true};
    m_nodeGItems.emplace(addedId, nodeG);
    addItem(nodeG);
    connect(nodeG, &NodeGraphicsItem::nodeSelected, this, &GraphScene::onNodeSelected);
}

void GraphScene::addEdge(uint32_t nodeIdFrom, uint32_t nodeIdTo)
{
    if (m_backend->edgeByNodes(nodeIdFrom, nodeIdTo))
        return;

    uint32_t addedId = m_backend->addEdge(nodeIdFrom, nodeIdTo).id;
    EdgeGraphicsItem *edgeG
        = new EdgeGraphicsItem{QLineF{mapGridToScenePos(m_backend->nodeById(nodeIdFrom)->pos),
                                      mapGridToScenePos(m_backend->nodeById(nodeIdTo)->pos)},
                               addedId};
    m_edgeGItems.emplace(addedId, edgeG);
    addItem(edgeG);
    connect(edgeG, &EdgeGraphicsItem::edgeSelected, this, &GraphScene::onEdgeSelected);
}

constexpr QPointF GraphScene::mapGridToScenePos(QPointF gridPos) noexcept
{
    return QPointF{gridPos.x() * (gridSize / gridTickStep), gridPos.y() * (gridSize / gridTickStep)};
}

constexpr QPointF GraphScene::mapSceneToGridPos(const QPointF &scenePos) noexcept
{
    return QPointF{scenePos.x() / (gridSize / gridTickStep),
                   scenePos.y() / (gridSize / gridTickStep)};
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

    int32_t left = qFloor(rect.left() / gridSize) * gridSize;
    int32_t top = qFloor(rect.top() / gridSize) * gridSize;

    // 100 is on stack but if you unzoom too much it moves to the heap
    QVarLengthArray<QLine, 100> lines;

    for (int32_t x = left; x <= rect.right(); x += gridSize) {
        lines.append(QLine(x, (int) rect.top(), x, (int) rect.bottom()));
    }

    for (int32_t y = top; y <= rect.bottom(); y += gridSize) {
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

    int32_t leftStart = qFloor(rect.left() / gridSize) * gridSize;
    int32_t topStart = qFloor(rect.top() / gridSize) * gridSize;

    for (int32_t x = leftStart; x <= rect.right(); x += gridSize) {
        painter->drawText(x + 2,
                          rect.bottom() - 15,
                          50,
                          20,
                          Qt::AlignLeft,
                          QString::number(x / gridSize * 10));
    }

    for (int32_t y = topStart; y <= rect.bottom(); y += gridSize) {
        painter->drawText(rect.right() - 40,
                          y + 2,
                          35,
                          20,
                          Qt::AlignRight,
                          QString::number(y / gridSize * -10));
    }
}
