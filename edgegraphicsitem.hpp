#ifndef EDGEGRAPHICSITEM_HPP
#define EDGEGRAPHICSITEM_HPP

#include <QGraphicsLineItem>

class NodeGraphicsItem;

class EdgeGraphicsItem : public QObject, public QGraphicsLineItem
{
    Q_OBJECT
public:
    explicit EdgeGraphicsItem(const QLineF &line,
                              NodeGraphicsItem *fromNodeG,
                              NodeGraphicsItem *toNodeG,
                              uint32_t backendEdgeId,
                              QObject *parent = nullptr);
    void updateGeometry();

signals:
    void edgeSelected(uint32_t edgeId);
    void edgeDeselected(uint32_t edgeId);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

private:
    NodeGraphicsItem *m_fromNodeG;
    NodeGraphicsItem *m_toNodeG;
    uint32_t m_backendEdgeId;
};

#endif // EDGEGRAPHICSITEM_HPP
