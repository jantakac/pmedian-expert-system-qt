#include "graph.hpp"

#include <QLineF>
#include <fstream>

Graph::Graph(QObject *parent)
    : QObject{parent}
{}

Graph::Graph(std::filesystem::path nodesPath, std::filesystem::path edgesPath, QObject *parent)
    : QObject{parent}
{
    loadNodes(nodesPath);
    loadEdges(edgesPath);
}

const Node &Graph::addNode(QPointF pos)
{
    auto it = m_nodes.emplace(m_nextNodeId, pos, m_nextNodeId);
    ++m_nextNodeId;
    return it.value();
}

void Graph::removeNode(uint32_t id)
{
    m_nodes.remove(id);
}

const Edge &Graph::addEdge(uint32_t from, uint32_t to)
{
    uint32_t id = from + (to << 16);
    auto it = m_edges.emplace(id,
                              id,
                              from,
                              to,
                              QLineF{nodeById(from)->pos, nodeById(to)->pos}.length());
    return it.value();
}

void Graph::removeEdge(uint32_t id)
{
    m_edges.remove(id);
}

const Edge *Graph::edgeByNodes(uint32_t fromNodeId, uint32_t toNodeId)
{
    auto it = m_edges.find(fromNodeId + (toNodeId << 16));
    if (it == m_edges.end())
        it = m_edges.find(toNodeId + (fromNodeId << 16));
    return it != m_edges.end() ? &(it.value()) : nullptr;
}

void Graph::editNode(const Node &node)
{
    Node *editedNode = nodeByIdEditable(node.id);
    editedNode->pos = node.pos;
    editedNode->type = node.type;
}

void Graph::editEdge(const Edge &edge)
{
    Edge *editedEdge = edgeByIdEditable(edge.id);
    editedEdge->length = edge.length;
    editedEdge->isEnabled = edge.isEnabled;
}

const Edge *Graph::edgeById(uint32_t id)
{
    return edgeByIdEditable(id);
}

const Node *Graph::nodeById(uint32_t id)
{
    return nodeByIdEditable(id);
}

void Graph::loadNodes(std::filesystem::path path)
{
    std::ifstream nodesStream{path};
    uint32_t id;
    int32_t x;
    int32_t y;
    // while (nodesStream >> id >> x >> y) {
    //     m_nodes.emplace_back(id, x, y);
    // };
}

void Graph::loadEdges(std::filesystem::path path)
{
    std::ifstream edgesStream{path};
    uint32_t from;
    uint32_t to;
    // while (edgesStream >> from >> to) {
    //     m_edges.emplace_back(from, to);
    // };
}

Node *Graph::nodeByIdEditable(uint32_t id)
{
    auto it = m_nodes.find(id);
    if (it == m_nodes.end())
        return nullptr;
    return &(it.value());
}

Edge *Graph::edgeByIdEditable(uint32_t id)
{
    auto it = m_edges.find(id);
    if (it == m_edges.end())
        return nullptr;
    return &(it.value());
}
