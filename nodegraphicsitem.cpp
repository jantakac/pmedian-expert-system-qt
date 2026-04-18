#include "nodegraphicsitem.hpp"

#include <QBrush>
#include <QPen>
#include <qfont.h>

#include "graphscene.hpp"

NodeGraphicsItem::NodeGraphicsItem(const QString &label,
                                   QPointF scenePos,
                                   uint32_t backendNodeId,
                                   uint16_t size,
                                   bool selectable,
                                   QGraphicsItem *parent)
    : QGraphicsEllipseItem(0, 0, size, size, parent)
    , m_label{new QGraphicsTextItem{label, this}}
    , m_backendNodeId{backendNodeId}
    , m_size{size}
{
    if (selectable)
        setFlags(ItemIsSelectable);
    setBrush(QBrush{Qt::green});
    setPen(QPen{Qt::black});
    setScenePos(scenePos);
}

void NodeGraphicsItem::setScenePos(const QPointF &scenePos) noexcept
{
    setRect(scenePos.x() - m_size / 2.0, scenePos.y() - m_size / 2.0, m_size, m_size);

    m_label->setPos(scenePos.x() - m_size / 2.0 + (m_size - m_label->boundingRect().width()) / 2.0,
                    scenePos.y() - m_size / 2.0 + (m_size - m_label->boundingRect().height()) / 2.0);
    m_label->font().setPointSize(m_size / 2.0);
}

uint32_t NodeGraphicsItem::backendNodeId()
{
    return m_backendNodeId;
}

void NodeGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsEllipseItem::mousePressEvent(event);

    if (isSelected()) {
        emit nodeSelected(m_backendNodeId);
    } else {
        emit nodeDeselected(m_backendNodeId);
    }
}
