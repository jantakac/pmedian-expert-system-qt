#include "nodegraphicsitem.hpp"
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include "edgegraphicsitem.hpp"

NodeGraphicsItem::NodeGraphicsItem(NodeId id, const Node &data)
    : m_id(id)
    , m_type(data.type)
    , m_visited(data.visited)
{
    setFlags(ItemIsMovable | ItemIsSelectable | ItemSendsGeometryChanges);
    setZValue(1.0); // Ensure nodes are always above edges
}

void NodeGraphicsItem::addConnectedEdge(EdgeGraphicsItem *edge)
{
    if (edge) {
        m_connectedEdges.push_back(edge);
    }
}

void NodeGraphicsItem::removeConnectedEdge(EdgeGraphicsItem *edge)
{
    std::erase(m_connectedEdges, edge);
}

void NodeGraphicsItem::updateFromModel(const Node &data)
{
    m_type = data.type;
    m_visited = data.visited;
    update();
}

QRectF NodeGraphicsItem::boundingRect() const
{
    const float margin = 2.0f;
    return QRectF(-Radius - margin, -Radius - margin, (Radius + margin) * 2, (Radius + margin) * 2);
}

void NodeGraphicsItem::paint(QPainter *painter,
                             const QStyleOptionGraphicsItem *option,
                             QWidget *widget)
{
    Q_UNUSED(widget);

    painter->setRenderHint(QPainter::Antialiasing);

    QBrush brush;
    if (m_type == NodeType::PMedianCandidate) {
        brush = QBrush(Qt::green);
    } else {
        brush = QBrush(m_visited ? Qt::darkBlue : Qt::blue);
    }

    QPen pen(Qt::black, 2);
    if (option->state & QStyle::State_Selected) {
        pen.setColor(Qt::red);
        pen.setWidth(3);
    }

    painter->setPen(pen);
    painter->setBrush(brush);
    painter->drawEllipse(QRectF(-Radius, -Radius, Radius * 2, Radius * 2));

    painter->setPen(Qt::white);
    painter->drawText(boundingRect(), Qt::AlignCenter, QString::number(static_cast<uint32_t>(m_id)));
}

QVariant NodeGraphicsItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionHasChanged) {
        for (auto *edge : m_connectedEdges) {
            edge->updatePosition();
        }
    }
    return QGraphicsObject::itemChange(change, value);
}