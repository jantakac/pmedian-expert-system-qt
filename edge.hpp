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
    bool isLengthManual;

    Edge(uint32_t id, uint32_t from, uint32_t to, float length)
        : id{id}
        , from{from}
        , to{to}
        , length{length}
        , isEnabled{true}
        , isLengthManual{false} {};
};

#endif // EDGE_HPP