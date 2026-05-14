#ifndef SASOLVER_HPP
#define SASOLVER_HPP

#include "Graph.hpp"
#include "OptimizationParams.hpp"
#include <unordered_map>
#include <vector>

struct SolverNode
{
    NodeId id;
    QPointF pos;
};

struct SolverEdge
{
    NodeId to;
    double length;
};

using AdjacencyList = std::unordered_map<NodeId, std::vector<SolverEdge>, NodeIdHash>;

struct SolverContext
{
    OptimizationParams params;
    std::vector<SolverNode> customers;
    std::vector<SolverNode> candidates;
    AdjacencyList adjList;
};

struct SolverResult
{
    std::vector<NodeId> chosenCandidates;
    std::vector<std::pair<NodeId, NodeId>> assignments;
    double finalCost{0.0};
};

class SASolver
{
public:
    [[nodiscard]] static SolverResult solve(const SolverContext &context);
};

#endif // SASOLVER_HPP
