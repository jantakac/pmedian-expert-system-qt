#ifndef NODEGRAPHICSITEM_HPP
#define NODEGRAPHICSITEM_HPP

#include <QGraphicsItem>

class NodeGraphicsItem : public QObject, public QGraphicsEllipseItem
{
    Q_OBJECT
public:
    explicit NodeGraphicsItem(const QString &label,
                              QPointF scenePos,
                              uint32_t backendNodeId,
                              uint16_t size,
                              bool selectable,
                              QGraphicsItem *parent = nullptr);
    void setScenePos(const QPointF &gridPos) noexcept;
    uint32_t backendNodeId();

signals:
    void nodeSelected(uint32_t nodeId);
    void nodeDeselected(uint32_t nodeId);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

private:
    QGraphicsTextItem *m_label;
    uint32_t m_backendNodeId;
    uint16_t m_size;
};

#endif // NODEGRAPHICSITEM_HPP
