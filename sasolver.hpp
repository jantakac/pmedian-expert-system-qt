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

private:
    struct DistanceView
    {
        double *data;
        size_t rows;
        size_t cols;
        [[nodiscard]] constexpr double &operator()(size_t r, size_t c) const noexcept
        {
            return data[r * cols + c];
        }
    };

    [[nodiscard]] static std::vector<double> computeDistanceMatrix(const SolverContext &context);

    [[nodiscard]] static double calculateCost(DistanceView dists,
                                              const std::vector<size_t> &state,
                                              double costPerKm) noexcept;

    [[nodiscard]] static std::vector<size_t> performOptimization(const SolverContext &context,
                                                                 DistanceView dists);

    [[nodiscard]] static SolverResult generateResult(const SolverContext &context,
                                                     DistanceView dists,
                                                     const std::vector<size_t> &bestState);
};

#endif // SASOLVER_HPP