#ifndef NODEGRAPHICSITEM_HPP
#define NODEGRAPHICSITEM_HPP

#include <QGraphicsItem>
#include <QObject>
#include "graphscene.hpp"

class EdgeGraphicsItem;
class Node;

class NodeGraphicsItem : public QObject, public QGraphicsEllipseItem
{
    Q_OBJECT
public:
    enum { Type = UserType + 1 };

    explicit NodeGraphicsItem(const QString &label,
                              QPointF scenePos,
                              uint32_t backendNodeId,
                              uint16_t size,
                              bool selectable,
                              QGraphicsItem *parent = nullptr);

    int type() const override { return Type; }

    uint32_t backendNodeId() const { return m_backendNodeId; }
    void addConnectedEdge(EdgeGraphicsItem *edgeG);
    void updateVisuals(const Node &node);
    const QList<EdgeGraphicsItem *> &connectedEdges() const { return m_connectedEdges; }

signals:
    void nodeSelected(uint32_t nodeId);
    void nodeDeselected(uint32_t nodeId);
    void nodeMoveFinished(uint32_t nodeId, QPointF newScenePos);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

private:
    QList<EdgeGraphicsItem *> m_connectedEdges;
    QPointF m_startingPos;
    QGraphicsTextItem *m_label;
    uint32_t m_backendNodeId;
    uint16_t m_size;

    void setupGeometry();
    void setupStyle();
    void setupInteraction(bool isSelectable);
    void setupLabel(const QString &text);
};

#endif // NODEGRAPHICSITEM_HPP