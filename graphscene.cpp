#include "graphscene.hpp"

#include <QPointer>
#include <QVarLengthArray>
#include <qpainter.h>

#include "nodegraphicsitem.hpp"

GraphScene::GraphScene(QObject *parent, Graph *backendGraph)
    : QGraphicsScene{parent}
{
    if (backendGraph) {
        setBackendGraph(backendGraph);
    }
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
    m_nodes.clear();
    NodeGraphicsItem *nodeG;
    for (const auto &node : m_backend->nodes()) {
        nodeG = new NodeGraphicsItem{QString::number(node.id), node.pos, nodeSize};
        m_nodes.emplaceBack(nodeG, &node);
    }

    // this is really unoptimized, should be better if kept
    // in a different datastructure for searching

    const NodeGraphicsItem *fromNodeG;
    const NodeGraphicsItem *toNodeG;
    QGraphicsLineItem *edgeG;
    for (const auto &edge : m_backend->edges()) {
        for (const auto &[nodeG, node] : std::as_const(m_nodes)) {
            if (node->id == edge.from)
                fromNodeG = nodeG;
            else if (node->id == edge.to)
                toNodeG = nodeG;
        }
        edgeG = new QGraphicsLineItem{fromNodeG->x() + nodeSize / 2.0,
                                      fromNodeG->y() + nodeSize / 2.0,
                                      toNodeG->x() + nodeSize / 2.0,
                                      toNodeG->y() + nodeSize / 2.0};
        edgeG->setPen(QPen{Qt::black, edgeWidth});
        m_edges.emplaceBack(edgeG, &edge);
        addItem(edgeG);
    }

    for (const auto &[nodeG, node] : std::as_const(m_nodes)) {
        addItem(nodeG);
    }
}

const QList<std::pair<NodeGraphicsItem *, const Graph::Node *> > &GraphScene::nodes() const
{
    return m_nodes;
}

const QList<std::pair<QGraphicsLineItem *, const Graph::Edge *> > &GraphScene::edges() const
{
    return m_edges;
}

void GraphScene::addNode(const QPointF &scenePos)
{
    const Graph::Node *addedNode = m_backend->addNode(mapSceneToGridPos(scenePos));
    m_nodes.emplaceBack(new NodeGraphicsItem{QString::number(addedNode->id), scenePos, 30},
                        addedNode);
    addItem(m_nodes.back().first);
}

constexpr QPointF GraphScene::mapGridToScenePos(const QPointF &gridPos) noexcept
{
    return QPointF{gridPos.x() * (gridSize / gridTickStep), gridPos.y() * (gridSize / gridTickStep)};
}

constexpr QPointF GraphScene::mapSceneToGridPos(const QPointF &scenePos) noexcept
{
    return QPointF{scenePos.x() / (gridSize / gridTickStep),
                   scenePos.y() / (gridSize / gridTickStep)};
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
