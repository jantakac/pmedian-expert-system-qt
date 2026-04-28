#ifndef NODE_HPP
#define NODE_HPP

#include <QPointF>

enum class NodeType : uint8_t { Customer, PMedianCandidate };

struct Node
{
    QPointF pos;
    uint32_t id;
    NodeType type;
    bool visited;
    Node(QPointF pos, uint32_t id)
        : pos{pos}
        , id{id}
        , type{NodeType::Customer}
        , visited{false} {};
};

#endif // NODE_HPP
