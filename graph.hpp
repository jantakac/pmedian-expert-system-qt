#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <QObject>
#include <filesystem>
#include <fstream>

class Graph : public QObject
{
    Q_OBJECT
public:
    struct Node
    {
        uint32_t id;
        int32_t x;
        int32_t y;
    };
    struct Edge
    {
        uint32_t from;
        uint32_t to;
    };
    explicit Graph(QObject *parent = nullptr);
    explicit Graph(std::filesystem::path nodesPath,
                   std::filesystem::path edgesPath,
                   QObject *parent = nullptr);

    const std::vector<Node> &nodes() const;
    const std::vector<Edge> &edges() const;

private:
    std::vector<Node> m_nodes;
    std::vector<Edge> m_edges;

    void loadNodes(std::filesystem::path path);
    void loadEdges(std::filesystem::path path);
};

#endif // GRAPH_HPP
