#include "nodegraphicsitem.hpp"

#include <QBrush>
#include <QPen>
#include <qfont.h>

#include "edgegraphicsitem.hpp"
#include "graphscene.hpp"

NodeGraphicsItem::NodeGraphicsItem(const QString &label,
                                   QPointF scenePos,
                                   uint32_t backendNodeId,
                                   uint16_t size,
                                   bool selectable,
                                   QGraphicsItem *parent)
    : QGraphicsEllipseItem(parent)
    , m_backendNodeId{backendNodeId}
    , m_size{size}
{
    setupGeometry();
    setupStyle();
    setupInteraction(selectable);
    setupLabel(label);
    setPos(scenePos);
}

void NodeGraphicsItem::setupGeometry()
{
    setRect(-m_size / 2.0, -m_size / 2.0, m_size, m_size);
}

void NodeGraphicsItem::setupStyle()
{
    setBrush(QBrush{Qt::green});
    setPen(QPen{Qt::black});
    setZValue(10);
}

void NodeGraphicsItem::setupInteraction(bool isSelectable)
{
    if (isSelectable)
        setFlags(ItemIsSelectable | ItemIsMovable | ItemSendsGeometryChanges);
}

void NodeGraphicsItem::setupLabel(const QString &text)
{
    m_label = new QGraphicsTextItem(text, this);

    QFont font = m_label->font();
    font.setPointSizeF(m_size / 3.0);
    m_label->setFont(font);

    QRectF lRect = m_label->boundingRect();
    m_label->setPos(-lRect.width() / 2.0, -lRect.height() / 2.0);
}

uint32_t NodeGraphicsItem::backendNodeId()
{
    return m_backendNodeId;
}

void NodeGraphicsItem::addConnectedEdge(EdgeGraphicsItem *edgeG)
{
    m_connectedEdges.push_back(edgeG);
}

void NodeGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsEllipseItem::mousePressEvent(event);

    if (isSelected())
        emit nodeSelected(m_backendNodeId);
    else
        emit nodeDeselected(m_backendNodeId);
}

QVariant NodeGraphicsItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionHasChanged) {
        for (EdgeGraphicsItem *connectedEdge : std::as_const(m_connectedEdges)) {
            connectedEdge->updateGeometry();
        }
    }
    return QGraphicsEllipseItem::itemChange(change, value);
}
