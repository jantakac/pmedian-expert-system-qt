#ifndef GRAPHSCENE_HPP
#define GRAPHSCENE_HPP

#include <QGraphicsScene>
#include <QPointer>
#include <QVarLengthArray>
#include <qpainter.h>

#include "graph.hpp"
#include "nodegraphicsitem.hpp"

class GraphScene : public QGraphicsScene
{
public:
    static constexpr uint8_t gridSize = 50;
    static constexpr uint8_t nodeSize = 40;
    static constexpr uint8_t edgeWidth = 8;
    explicit GraphScene(QObject *parent = nullptr, Graph *backendGraph = nullptr);
    Graph &backend();
    void setBackendGraph(Graph *backendGraph);
    const QList<std::pair<NodeGraphicsItem *, const Graph::Node *>> &nodes() const;
    const QList<std::pair<QGraphicsLineItem *, const Graph::Edge *>> &edges() const;

public slots:
    void onGraphDataChanged();

protected:
    void drawBackground(QPainter *painter, const QRectF &rect) override;
    void drawForeground(QPainter *painter, const QRectF &rect) override;

private:
    QList<std::pair<NodeGraphicsItem *, const Graph::Node *>> m_nodes;
    QList<std::pair<QGraphicsLineItem *, const Graph::Edge *>> m_edges;
    QPointer<Graph> m_backend;
};

#endif // GRAPHSCENE_HPP
