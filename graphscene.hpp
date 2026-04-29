#ifndef GRAPHSCENE_HPP
#define GRAPHSCENE_HPP

#include <QGraphicsScene>
#include <QHash>
#include <QPointer>

class NodeGraphicsItem;
class EdgeGraphicsItem;
class Graph;
class Node;
class Edge;

class GraphScene : public QGraphicsScene
{
    Q_OBJECT
public:
    static constexpr uint8_t nodeSize = 40;
    static constexpr uint8_t edgeWidth = 8;

    explicit GraphScene(QObject *parent = nullptr, Graph *backendGraph = nullptr);
    void setBackendGraph(Graph *backendGraph);
    const QList<std::pair<NodeGraphicsItem *, const Node *>> &nodes() const;
    const QList<std::pair<EdgeGraphicsItem *, const Edge *>> &edges() const;
    void addNode(const QPointF &scenePos);
    void addEdge(uint32_t nodeIdFrom, uint32_t nodeIdTo);
    void updateNode(const Node &node);
    void updateEdge(const Edge &edge);
    const Node &backendNodeById(uint32_t id);
    const Edge &backendEdgeById(uint32_t id);
    void showPreviewNode();
    void hidePreviewNode();
    void setPreviewNodePos(const QPointF &pos);

public slots:
    void onNodeSelected(uint32_t nodeId);
    void onEdgeSelected(uint32_t edgeId);

protected:
    void drawBackground(QPainter *painter, const QRectF &rect) override;
    void drawForeground(QPainter *painter, const QRectF &rect) override;

private:
    QHash<uint32_t, NodeGraphicsItem *> m_nodeGItems;
    QHash<uint32_t, EdgeGraphicsItem *> m_edgeGItems;

    NodeGraphicsItem *m_previewNode;
    QPointer<Graph> m_backend;

    uint32_t mapSceneToGridPos(const QPointF &);
    void updateAutoEdgeLengthsForNode(uint32_t nodeId);
private slots:
    void handleNodeMoveFinished(uint32_t nodeId, QPointF newScenePos);
};

#endif // GRAPHSCENE_HPP
