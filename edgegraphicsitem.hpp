#ifndef EDGEGRAPHICSITEM_HPP
#define EDGEGRAPHICSITEM_HPP

#include <QGraphicsObject>
#include "graph.hpp"

class NodeGraphicsItem;

class EdgeGraphicsItem : public QGraphicsObject
{
    Q_OBJECT
public:
    EdgeGraphicsItem(EdgeId id, NodeGraphicsItem *fromNode, NodeGraphicsItem *toNode);
    ~EdgeGraphicsItem() override;

    EdgeId id() const { return m_id; }

    void updateFromModel(const Edge &data);

    void updatePosition();

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    QPainterPath shape() const override;

private:
    const EdgeId m_id;
    NodeGraphicsItem *m_fromNode;
    NodeGraphicsItem *m_toNode;

    QLineF m_line{};
    QRectF m_boundingRect{};

    float m_length{0.0f};
    bool m_isEnabled{true};
    bool m_isManual{false};

    static constexpr float StrokeWidth = 4.0f;
    static constexpr float SelectionTolerance = 10.0f;

    QPen determinePen(const QStyleOptionGraphicsItem *option) const;
    void drawLabel(QPainter *painter) const;
};

#endif // EDGEGRAPHICSITEM_HPP