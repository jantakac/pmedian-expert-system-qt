#ifndef NODEGRAPHICSITEM_HPP
#define NODEGRAPHICSITEM_HPP

#include <QGraphicsItem>

class NodeGraphicsItem : public QGraphicsEllipseItem
{
public:
    explicit NodeGraphicsItem(const QString &label,
                              QPointF scenePos,
                              uint16_t size,
                              QGraphicsItem *parent = nullptr);
    void setScenePos(const QPointF &gridPos) noexcept;

private:
    QGraphicsTextItem *m_label;
    uint16_t m_size;
};

#endif // NODEGRAPHICSITEM_HPP
