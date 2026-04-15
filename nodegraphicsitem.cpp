#include "nodegraphicsitem.hpp"

#include <QBrush>
#include <QPen>
#include <qfont.h>

NodeGraphicsItem::NodeGraphicsItem(const QString &label,
                                   QPointF scenePos,
                                   uint16_t size,
                                   QGraphicsItem *parent)
    : QGraphicsEllipseItem(0, 0, size, size, parent)
    , m_label{new QGraphicsTextItem{label, this}}
    , m_size{size}
{
    setFlags(ItemIsSelectable);
    setBrush(QBrush{Qt::green});
    setPen(QPen{Qt::black});
    m_label->font().setPointSize(m_size / 2.0);
    setScenePos(scenePos);
}

void NodeGraphicsItem::setScenePos(const QPointF &scenePos) noexcept
{
    setRect(scenePos.x() - m_size / 2.0, scenePos.y() - m_size / 2.0, m_size, m_size);

    m_label->setPos(scenePos.x() - m_size / 2.0 + (m_size - m_label->boundingRect().width()) / 2.0,
                    scenePos.y() - m_size / 2.0 + (m_size - m_label->boundingRect().height()) / 2.0);
}
