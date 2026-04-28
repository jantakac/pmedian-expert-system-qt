#ifndef EDGE_HPP
#define EDGE_HPP

#include <cstdint>

struct Edge
{
    uint32_t id;
    uint32_t from;
    uint32_t to;
    float length;
    bool isEnabled;
    Edge(uint32_t id, uint32_t from, uint32_t to, float length)
        : id{id}
        , from{from}
        , to{to}
        , length{length}
        , isEnabled{true} {};
};

#endif // EDGE_HPP