#include "graph.hpp"
#include <cmath>

NodeId Graph::addNode(QPointF pos)
{
    NodeId id = static_cast<NodeId>(m_nextNodeId++);
    m_nodes[id] = Node{.id = id, .pos = pos, .type = NodeType::Customer, .visited = false};
    emit nodeAdded(id);
    return id;
}

void Graph::removeNode(NodeId id)
{
    if (!m_nodes.contains(id))
        return;

    std::vector<EdgeId> edgesToRemove;
    for (const auto &[edgeId, edge] : m_edges) {
        if (edge.from == id || edge.to == id) {
            edgesToRemove.push_back(edgeId);
        }
    }

    for (EdgeId eId : edgesToRemove) {
        removeEdge(eId);
    }

    m_nodes.erase(id);
    emit nodeRemoved(id);
}

void Graph::moveNode(NodeId id, QPointF newPos)
{
    auto it = m_nodes.find(id);
    if (it == m_nodes.end())
        return;

    it->second.pos = newPos;
    emit nodeMoved(id, newPos);

    for (auto &[edgeId, edge] : m_edges) {
        if ((edge.from == id || edge.to == id) && !edge.isLengthManual) {
            edge.length = calculateDistance(m_nodes[edge.from].pos, m_nodes[edge.to].pos);
            emit edgeUpdated(edgeId);
        }
    }
}

void Graph::updateNode(const Node &node)
{
    auto it = m_nodes.find(node.id);
    if (it == m_nodes.end())
        return;

    bool posChanged = (it->second.pos != node.pos);
    it->second = node;

    if (posChanged) {
        moveNode(node.id, node.pos);
    } else {
        emit nodeUpdated(node.id);
    }
}

void Graph::setNodeType(NodeId id, NodeType type)
{
    auto it = m_nodes.find(id);
    if (it != m_nodes.end() && it->second.type != type) {
        it->second.type = type;
        emit nodeUpdated(id);
    }
}

EdgeId Graph::addEdge(NodeId from, NodeId to, std::optional<float> manualLength)
{
    if (from == to || !m_nodes.contains(from) || !m_nodes.contains(to)) {
        return EdgeId::Invalid;
    }

    for (const auto &[id, edge] : m_edges) {
        if ((edge.from == from && edge.to == to) || (edge.from == to && edge.to == from)) {
            return id;
        }
    }

    EdgeId id = static_cast<EdgeId>(m_nextEdgeId++);
    float length = manualLength.has_value() ? *manualLength
                                            : calculateDistance(m_nodes[from].pos, m_nodes[to].pos);

    m_edges[id] = Edge{.id = id,
                       .from = from,
                       .to = to,
                       .length = length,
                       .isEnabled = true,
                       .isLengthManual = manualLength.has_value()};

    emit edgeAdded(id);
    return id;
}

void Graph::removeEdge(EdgeId id)
{
    if (m_edges.erase(id)) {
        emit edgeRemoved(id);
    }
}

void Graph::updateEdge(const Edge &edge)
{
    if (m_edges.contains(edge.id)) {
        m_edges[edge.id] = edge;
        emit edgeUpdated(edge.id);
    }
}

const Node *Graph::findNode(NodeId id) const
{
    auto it = m_nodes.find(id);
    return (it != m_nodes.end()) ? &it->second : nullptr;
}

const Edge *Graph::findEdge(EdgeId id) const
{
    auto it = m_edges.find(id);
    return (it != m_edges.end()) ? &it->second : nullptr;
}

float Graph::calculateDistance(QPointF a, QPointF b) const
{
    return static_cast<float>(std::hypot(a.x() - b.x(), a.y() - b.y()));
}