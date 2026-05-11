#ifndef SOLVERTYPES_HPP
#define SOLVERTYPES_HPP

#include "graph.hpp"

struct SolverNode
{
    NodeId id;
    QPointF pos;
};

struct SolverResult
{
    std::vector<NodeId> chosenCandidates;
};

#endif // SOLVERTYPES_HPP
