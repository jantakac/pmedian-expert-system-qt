#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <QHash>
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

        Node(QPointF pos, uint32_t id)
            : pos{pos}
            , id{id} {};
    };
    struct Edge
    {
        uint32_t id;
        uint32_t from;
        uint32_t to;
        float length;
        Edge(uint32_t id, uint32_t from, uint32_t to, float length)
            : id{id}
            , from{from}
            , to{to}
            , length{length} {};
    };
    explicit Graph(QObject *parent = nullptr);
    explicit Graph(std::filesystem::path nodesPath,
                   std::filesystem::path edgesPath,
                   QObject *parent = nullptr);
    const Node &addNode(QPointF pos);
    void removeNode(uint32_t id);
    const Edge &addEdge(uint32_t from, uint32_t to);
    void removeEdge(uint32_t id);
    const Node *nodeById(uint32_t id);
    const Edge *edgeById(uint32_t id);
    const Edge *edgeByNodes(uint32_t fromNodeId, uint32_t toNodeId);

private:
    QHash<std::uint32_t, Node> m_nodes;
    QHash<std::uint32_t, Edge> m_edges;
    double m_costPerKm = 0;
    uint32_t m_nextNodeId = 1;

    void loadNodes(std::filesystem::path path);
    void loadEdges(std::filesystem::path path);
};

#endif // GRAPH_HPP
