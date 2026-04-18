#include "edgegraphicsitem.hpp"

#include "graphscene.hpp"

EdgeGraphicsItem::EdgeGraphicsItem(const QLineF &line, uint32_t backendEdgeId, QObject *parent)
    : QObject{parent}
    , QGraphicsLineItem{line}
    , m_backendEdgeId{backendEdgeId}
{
    setFlags(ItemIsSelectable);
    setPen(QPen{Qt::black, GraphScene::edgeWidth});
}

void EdgeGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsLineItem::mousePressEvent(event);

    if (isSelected())
        emit edgeSelected(m_backendEdgeId);
    else
        emit edgeDeselected(m_backendEdgeId);
}
