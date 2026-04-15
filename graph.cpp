#include "graph.hpp"

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

const Graph::Node *Graph::addNode(QPointF pos)
{
    m_nodes.emplace_back(pos, ++m_lastAddedNodeId);
    return &m_nodes.back();
}

const Graph::Edge *Graph::addEdge(uint32_t from, uint32_t to, uint32_t length)
{
    m_edges.emplace_back(from, to, length);
    return &m_edges.back();
}

const std::vector<Graph::Node> &Graph::nodes() const
{
    return m_nodes;
}

const std::vector<Graph::Edge> &Graph::edges() const
{
    return m_edges;
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
