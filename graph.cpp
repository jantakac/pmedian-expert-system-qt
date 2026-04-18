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

const Graph::Node &Graph::addNode(QPointF pos)
{
    auto it = m_nodes.emplace(m_nextNodeId, pos, m_nextNodeId);
    ++m_nextNodeId;
    return it.value();
}

void Graph::removeNode(uint32_t id)
{
    m_nodes.remove(id);
}

const Graph::Edge &Graph::addEdge(uint32_t from, uint32_t to)
{
    auto it = m_edges.emplace(m_nextEdgeId,
                              m_nextEdgeId,
                              from,
                              to,
                              QLineF{nodeById(from)->pos, nodeById(to)->pos}.length());
    ++m_nextEdgeId;
    return it.value();
}

void Graph::removeEdge(uint32_t id)
{
    m_edges.remove(id);
}

const Graph::Edge *Graph::edgeById(uint32_t id)
{
    auto it = m_edges.find(id);
    if (it == m_edges.end())
        return nullptr;
    return &(it.value());
}

const Graph::Node *Graph::nodeById(uint32_t id)
{
    auto it = m_nodes.find(id);
    if (it == m_nodes.end())
        return nullptr;
    return &(it.value());
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
