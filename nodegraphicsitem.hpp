#ifndef NODEGRAPHICSITEM_HPP
#define NODEGRAPHICSITEM_HPP

#include <QGraphicsObject>
#include "graph.hpp"
#include <vector>

class EdgeGraphicsItem;

class NodeGraphicsItem : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit NodeGraphicsItem(NodeId id, const Node &data);

    NodeId id() const { return m_id; }

    void addConnectedEdge(EdgeGraphicsItem *edge);
    void removeConnectedEdge(EdgeGraphicsItem *edge);

    void updateFromModel(const Node &data);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    void setPreview(bool enabled);

signals:
    void positionChanged(NodeId id, QPointF newPos);

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

private:
    const NodeId m_id;
    NodeType m_type;
    bool m_isPreview{false};

    std::vector<EdgeGraphicsItem *> m_connectedEdges;

    static constexpr float Radius = 20.0f;
};

#endif // NODEGRAPHICSITEM_HPP