#include "edgegraphicsitem.hpp"

#include "graphscene.hpp"
#include "nodegraphicsitem.hpp"

EdgeGraphicsItem::EdgeGraphicsItem(const QLineF &line,
                                   NodeGraphicsItem *fromNodeG,
                                   NodeGraphicsItem *toNodeG,
                                   uint32_t backendEdgeId,
                                   float initialLength,
                                   QObject *parent)
    : QObject{parent}
    , QGraphicsLineItem{line}
    , m_fromNodeG{fromNodeG}
    , m_toNodeG{toNodeG}
    , m_backendEdgeId{backendEdgeId}
{
    setFlags(ItemIsSelectable);
    setZValue(5);

    m_label = new QGraphicsTextItem(this);
    m_label->setPlainText(QString::fromStdString(std::format("{:.2f}", initialLength)));

    QFont font = m_label->font();
    font.setPointSizeF(10);
    font.setBold(true);
    m_label->setFont(font);

    updateVisuals(Edge(backendEdgeId, 0, 0, initialLength));
}

void EdgeGraphicsItem::updateGeometry()
{
    QLineF newLine{m_fromNodeG->mapToScene(m_fromNodeG->rect().center()),
                   m_toNodeG->mapToScene(m_toNodeG->rect().center())};
    setLine(newLine);
    updateLabelPosition();
}

void EdgeGraphicsItem::updateVisuals(const Edge &edge)
{
    m_label->setPlainText(QString::fromStdString(std::format("{:.2f}", edge.length)));

    if (!edge.isEnabled) {
        QColor disabledColor(200, 200, 200, 80);
        setPen(QPen{disabledColor, GraphScene::edgeWidth});
        m_label->setDefaultTextColor(disabledColor);
    } else {
        setPen(QPen{Qt::black, GraphScene::edgeWidth});
        m_label->setDefaultTextColor(Qt::darkBlue);
    }

    updateGeometry();
}

void EdgeGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsLineItem::mousePressEvent(event);

    if (isSelected())
        emit edgeSelected(m_backendEdgeId);
    else
        emit edgeDeselected(m_backendEdgeId);
}

void EdgeGraphicsItem::updateLabelPosition()
{
    QLineF curLine = line();
    QPointF center = curLine.center();

    qreal angle = -curLine.angle();

    if (curLine.angle() > 90 && curLine.angle() < 270) {
        angle += 180;
    }
    m_label->setRotation(angle);

    qreal dx = curLine.dx();
    qreal dy = curLine.dy();
    qreal len = curLine.length();

    if (len == 0)
        return;

    const qreal offset = 20.0;
    QPointF normal(-dy / len * offset, dx / len * offset);

    QRectF labelRect = m_label->boundingRect();
    m_label->setTransformOriginPoint(labelRect.center());

    QPointF finalPos = center + normal - labelRect.center();
    m_label->setPos(finalPos);
}
