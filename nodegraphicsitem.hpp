#ifndef NODEGRAPHICSITEM_HPP
#define NODEGRAPHICSITEM_HPP

#include <QGraphicsItem>

class EdgeGraphicsItem;

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
    uint32_t backendNodeId();
    void addConnectedEdge(EdgeGraphicsItem *edgeG);

signals:
    void nodeSelected(uint32_t nodeId);
    void nodeDeselected(uint32_t nodeId);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

private:
    QList<EdgeGraphicsItem *> m_connectedEdges;
    QGraphicsTextItem *m_label;
    uint32_t m_backendNodeId;
    uint16_t m_size;

    void setupGeometry();
    void setupStyle();
    void setupInteraction(bool isSelectable);
    void setupLabel(const QString &text);
};

#endif // NODEGRAPHICSITEM_HPP
