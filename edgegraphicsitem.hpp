#ifndef EDGEGRAPHICSITEM_HPP
#define EDGEGRAPHICSITEM_HPP

#include <QGraphicsLineItem>
#include <QGraphicsTextItem>

#include "edge.hpp"

class NodeGraphicsItem;

class EdgeGraphicsItem : public QObject, public QGraphicsLineItem
{
    Q_OBJECT
public:
    explicit EdgeGraphicsItem(const QLineF &line,
                              NodeGraphicsItem *fromNodeG,
                              NodeGraphicsItem *toNodeG,
                              uint32_t backendEdgeId,
                              float initialLength,
                              QObject *parent = nullptr);
    void updateGeometry();
    void updateVisuals(const Edge &edge);
    uint32_t backendEdgeId() const { return m_backendEdgeId; }

signals:
    void edgeSelected(uint32_t edgeId);
    void edgeDeselected(uint32_t edgeId);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

private:
    NodeGraphicsItem *m_fromNodeG;
    NodeGraphicsItem *m_toNodeG;
    QGraphicsTextItem *m_label;
    uint32_t m_backendEdgeId;

    void updateLabelPosition();
};

#endif // EDGEGRAPHICSITEM_HPP
