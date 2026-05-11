#ifndef GRAPHSCENE_HPP
#define GRAPHSCENE_HPP

#include <QGraphicsScene>
#include "graph.hpp"
#include <unordered_map>

class NodeGraphicsItem;
class EdgeGraphicsItem;

enum class InteractionMode { Select, Pan, PlaceNode, PlaceEdge, Edit, Delete };

class GraphScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit GraphScene(Graph *graph, QObject *parent = nullptr);

    void setInteractionMode(InteractionMode mode);

signals:
    void deletionRequested(NodeId id, int edgeCount);
    void nodeEditRequested(NodeId id);
    void edgeEditRequested(EdgeId id);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void drawBackground(QPainter *painter, const QRectF &rect) override;
    void drawForeground(QPainter *painter, const QRectF &rect) override;

private slots:
    void handleNodeAdded(NodeId id);
    void handleNodeRemoved(NodeId id);
    void handleNodeMoved(NodeId id, QPointF pos);
    void handleNodeUpdated(NodeId id);
    void handleEdgeAdded(EdgeId id);
    void handleEdgeRemoved(EdgeId id);
    void handleEdgeUpdated(EdgeId id);

private:
    Graph *m_graph;

    InteractionMode m_mode{InteractionMode::Select};
    std::optional<NodeId> m_firstNodeForEdge;

    std::unordered_map<NodeId, NodeGraphicsItem *> m_nodeItems;
    std::unordered_map<EdgeId, EdgeGraphicsItem *> m_edgeItems;

    NodeGraphicsItem *m_previewNode;

    void updatePreview(QPointF scenePos);
};

#endif // GRAPHSCENE_HPP