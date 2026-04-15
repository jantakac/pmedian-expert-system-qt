#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <QObject>
#include <QPointF>
#include <filesystem>

class Graph : public QObject
{
    Q_OBJECT
public:
    struct Node
    {
        QPointF pos;
        uint32_t id;
    };
    struct Edge
    {
        uint32_t from;
        uint32_t to;
        uint32_t length;
    };
    explicit Graph(QObject *parent = nullptr);
    explicit Graph(std::filesystem::path nodesPath,
                   std::filesystem::path edgesPath,
                   QObject *parent = nullptr);

    const std::vector<Node> &nodes() const;
    const std::vector<Edge> &edges() const;

    const Node *addNode(QPointF pos);
    const Edge *addEdge(uint32_t from, uint32_t to, uint32_t length);

private:
    std::vector<Node> m_nodes;
    std::vector<Edge> m_edges;
    uint32_t m_lastAddedNodeId = 0;
    uint32_t m_lastAddedEdgeId = 0;

    void loadNodes(std::filesystem::path path);
    void loadEdges(std::filesystem::path path);
};

#endif // GRAPH_HPP
