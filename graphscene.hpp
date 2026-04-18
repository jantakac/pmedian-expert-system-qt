#ifndef GRAPHSCENE_HPP
#define GRAPHSCENE_HPP

#include <QGraphicsScene>
#include <QPointer>

#include "graph.hpp"
#include "nodegraphicsitem.hpp"

class NodeGraphicsItem;
class EdgeGraphicsItem;

class GraphScene : public QGraphicsScene
{
    Q_OBJECT
public:
    static constexpr uint8_t gridSize = 50;
    static constexpr uint8_t gridTickStep = 10;
    static constexpr uint8_t nodeSize = 40;
    static constexpr uint8_t edgeWidth = 8;

    explicit GraphScene(QObject *parent = nullptr, Graph *backendGraph = nullptr);
    Graph &backend();
    void setBackendGraph(Graph *backendGraph);
    const QList<std::pair<NodeGraphicsItem *, const Graph::Node *>> &nodes() const;
    const QList<std::pair<EdgeGraphicsItem *, const Graph::Edge *>> &edges() const;
    void addNode(const QPointF &scenePos);
    void addEdge(uint32_t nodeIdFrom, uint32_t nodeIdTo);
    static constexpr QPointF mapGridToScenePos(QPointF gridPos) noexcept;
    static constexpr QPointF mapSceneToGridPos(const QPointF &scenePos) noexcept;
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

    NodeGraphicsItem *m_previewNode = new NodeGraphicsItem{"", QPointF{0, 0}, 0, nodeSize, false};
    QPointer<Graph> m_backend;
};

#endif // GRAPHSCENE_HPP
