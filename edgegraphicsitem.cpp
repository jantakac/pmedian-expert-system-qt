#include "edgegraphicsitem.hpp"

#include "graphscene.hpp"
#include "nodegraphicsitem.hpp"

EdgeGraphicsItem::EdgeGraphicsItem(const QLineF &line,
                                   NodeGraphicsItem *fromNodeG,
                                   NodeGraphicsItem *toNodeG,
                                   uint32_t backendEdgeId,
                                   QObject *parent)
    : QObject{parent}
    , QGraphicsLineItem{line}
    , m_fromNodeG{fromNodeG}
    , m_toNodeG{toNodeG}
    , m_backendEdgeId{backendEdgeId}
{
    setFlags(ItemIsSelectable);
    setPen(QPen{Qt::black, GraphScene::edgeWidth});
    updateGeometry();
}

void EdgeGraphicsItem::updateGeometry()
{
    setLine(QLineF{m_fromNodeG->mapToScene(m_fromNodeG->rect().center()),
                   m_toNodeG->mapToScene(m_toNodeG->rect().center())});
}

void EdgeGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsLineItem::mousePressEvent(event);

    if (isSelected())
        emit edgeSelected(m_backendEdgeId);
    else
        emit edgeDeselected(m_backendEdgeId);
}
