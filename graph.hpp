#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <QObject>
#include <QPointF>
#include <cstdint>
#include <optional>
#include <unordered_map>

enum class NodeId : uint32_t { Invalid = 0 };
enum class EdgeId : uint32_t { Invalid = 0 };

struct NodeIdHash
{
    size_t operator()(NodeId id) const { return static_cast<size_t>(id); }
};
struct EdgeIdHash
{
    size_t operator()(EdgeId id) const { return static_cast<size_t>(id); }
};

enum class NodeType : uint8_t { Customer, PMedianCandidate, ChosenMedian };

struct Node
{
    NodeId id;
    QPointF pos; // Grid coordinates
    NodeType type{NodeType::Customer};
    bool visited{false};
};

struct Edge
{
    EdgeId id;
    NodeId from;
    NodeId to;
    float length;
    bool isEnabled{true};
    bool isLengthManual{false};
    bool isSolverOutput{false};
};

class Graph : public QObject
{
    Q_OBJECT
public:
    explicit Graph(QObject *parent = nullptr)
        : QObject(parent)
    {}

    NodeId addNode(QPointF pos);
    void removeNode(NodeId id);
    void moveNode(NodeId id, QPointF newPos);
    void updateNode(const Node &node);
    void setNodeType(NodeId id, NodeType type);

    EdgeId addEdge(NodeId from, NodeId to, std::optional<float> manualLength = std::nullopt);
    void addSolutionEdge(NodeId from, NodeId to);
    void removeEdge(EdgeId id);
    void updateEdge(const Edge &edge);

    void removeSolution();

    [[nodiscard]] const Node *findNode(NodeId id) const;
    [[nodiscard]] const Edge *findEdge(EdgeId id) const;
    [[nodiscard]] const Edge lastAddedSolutionEdge() const;
    [[nodiscard]] const std::unordered_map<NodeId, Node, NodeIdHash> &nodes() const
    {
        return m_nodes;
    }
    [[nodiscard]] const std::unordered_map<EdgeId, Edge, EdgeIdHash> &edges() const
    {
        return m_edges;
    }
    [[nodiscard]] bool isFullyConnected() const;
    [[nodiscard]] bool save(const QString &path) const;
    [[nodiscard]] bool load(const QString &path);
    void clear();

signals:
    void nodeAdded(NodeId id);
    void nodeRemoved(NodeId id);
    void nodeMoved(NodeId id, QPointF newPos);
    void nodeUpdated(NodeId id);

    void edgeAdded(EdgeId id);
    void solutionEdgeAdded();
    void edgeRemoved(EdgeId id);
    void solutionEdgesRemoved();
    void edgeUpdated(EdgeId id);

private:
    std::unordered_map<NodeId, Node, NodeIdHash> m_nodes;
    std::unordered_map<EdgeId, Edge, EdgeIdHash> m_edges;
    std::vector<Edge> m_solEdges;

    uint32_t m_nextNodeId{1};
    uint32_t m_nextEdgeId{1};

    float calculateDistance(QPointF a, QPointF b) const;
};

#endif // GRAPH_HPP