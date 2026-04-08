#include "nodegraphicsitem.hpp"

NodeGraphicsItem::NodeGraphicsItem(
    const QString &label, int32_t x, int32_t y, uint16_t size, QGraphicsItem *parent)
    : QGraphicsEllipseItem(0, 0, size, size, parent)
    , m_label{new QGraphicsTextItem{label, this}}
    , m_size{size}
{
    setFlags(ItemIsSelectable);
    setBrush(QBrush{Qt::green});
    setPen(QPen{Qt::black});
    changePosOnGrid(x, y);
    updateLabelPos();
}

void NodeGraphicsItem::updateLabelPos()
{
    if (!m_label)
        return;

    QFont font = m_label->font();
    font.setPointSize(m_size / 2.0);
    m_label->setFont(font);

    m_label->setPos(m_size / 2.0 - m_label->boundingRect().width() / 2,
                    m_size / 2.0 - m_label->boundingRect().height() / 2);
}

void NodeGraphicsItem::changePosOnGrid(int32_t xGrid, int32_t yGrid)
{
    setPos(xGrid * 5 - m_size / 2.0, yGrid * -5 - m_size / 2.0);
}
