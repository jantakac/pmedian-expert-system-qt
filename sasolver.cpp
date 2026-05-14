#include "SASolver.hpp"
#include <algorithm>
#include <cmath>
#include <limits>
#include <numeric>
#include <queue>
#include <random>

SolverResult SASolver::solve(const SolverContext &context)
{
    if (context.customers.empty() || context.candidates.empty() || context.params.pMedians <= 0) {
        return SolverResult{};
    }

    std::vector<double> distData{computeDistanceMatrix(context)};
    DistanceView dists{distData.data(), context.customers.size(), context.candidates.size()};

    const std::vector<size_t> bestIndices{performOptimization(context, dists)};
    return generateResult(context, dists, bestIndices);
}

std::vector<double> SASolver::computeDistanceMatrix(const SolverContext &context)
{
    const auto numCust{context.customers.size()};
    const auto numCand{context.candidates.size()};
    constexpr double INF{std::numeric_limits<double>::infinity()};

    std::vector<double> storage(numCust * numCand, INF);
    DistanceView dists{storage.data(), numCust, numCand};

    std::unordered_map<NodeId, size_t, NodeIdHash> customerIdToIndex;
    customerIdToIndex.reserve(numCust);
    for (size_t i{0}; i < numCust; ++i) {
        customerIdToIndex[context.customers[i].id] = i;
    }

    using PQElement = std::pair<double, NodeId>;

    for (size_t j{0}; j < numCand; ++j) {
        std::unordered_map<NodeId, double, NodeIdHash> minDists;
        std::priority_queue<PQElement, std::vector<PQElement>, std::greater<>> pq;

        const NodeId startNode{context.candidates[j].id};
        minDists[startNode] = 0.0;
        pq.push({0.0, startNode});

        while (!pq.empty()) {
            const auto [currDist, currNode]{pq.top()};
            pq.pop();

            if (currDist > minDists[currNode])
                continue;

            if (auto it{context.adjList.find(currNode)}; it != context.adjList.end()) {
                for (const auto &edge : it->second) {
                    const double newDist{currDist + edge.length};
                    if (!minDists.contains(edge.to) || newDist < minDists[edge.to]) {
                        minDists[edge.to] = newDist;
                        pq.push({newDist, edge.to});
                    }
                }
            }
        }

        for (const auto &[nodeId, dist] : minDists) {
            if (auto it{customerIdToIndex.find(nodeId)}; it != customerIdToIndex.end()) {
                dists(it->second, j) = dist;
            }
        }
    }
    return storage;
}

double SASolver::calculateCost(DistanceView dists,
                               const std::vector<size_t> &state,
                               double costPerKm) noexcept
{
    double totalCost{0.0};
    const auto numCust{dists.rows};
    constexpr double INF{std::numeric_limits<double>::infinity()};

    for (size_t i{0}; i < numCust; ++i) {
        double minDist{INF};
        for (const size_t candIdx : state) {
            minDist = std::min(minDist, dists(i, candIdx));
        }
        totalCost += (minDist == INF) ? 0.0 : minDist;
    }
    return totalCost * costPerKm;
}

std::vector<size_t> SASolver::performOptimization(const SolverContext &context, DistanceView dists)
{
    const auto numCand{context.candidates.size()};
    const auto p{std::min(static_cast<size_t>(context.params.pMedians), numCand)};
    const auto &params{context.params};

    std::vector<size_t> currentState(p);
    std::iota(currentState.begin(), currentState.end(), 0);

    if (p == numCand)
        return currentState;

    std::mt19937 rng{std::random_device{}()};
    std::uniform_real_distribution<double> unitDist{0.0, 1.0};
    std::uniform_int_distribution<size_t> pDist{0, p - 1};
    std::uniform_int_distribution<size_t> candDist{0, numCand - 1};

    std::vector<size_t> bestState{currentState};
    double currentCost{calculateCost(dists, currentState, params.costPerKm)};
    double bestCost{currentCost};

    double temp{params.tMax};
    while (temp > params.tMin) {
        for (int i{0}; i < params.iterationsPerStep; ++i) {
            std::vector<size_t> nextState{currentState};
            const size_t swapIdx{pDist(rng)};

            size_t newCand;
            do {
                newCand = candDist(rng);
            } while (std::ranges::contains(nextState, newCand));

            nextState[swapIdx] = newCand;
            const double nextCost{calculateCost(dists, nextState, params.costPerKm)};
            const double delta{nextCost - currentCost};

            if (delta < 0.0 || unitDist(rng) < std::exp(-delta / temp)) {
                currentState = std::move(nextState);
                currentCost = nextCost;

                if (currentCost < bestCost) {
                    bestCost = currentCost;
                    bestState = currentState;
                }
            }
        }
        temp *= params.coolingFactor;
    }

    return bestState;
}

SolverResult SASolver::generateResult(const SolverContext &context,
                                      DistanceView dists,
                                      const std::vector<size_t> &bestState)
{
    const auto numCust{context.customers.size()};
    const auto &customers{context.customers};
    const auto &candidates{context.candidates};

    SolverResult result;
    result.chosenCandidates.reserve(bestState.size());
    for (const size_t idx : bestState) {
        result.chosenCandidates.push_back(candidates[idx].id);
    }

    result.assignments.reserve(numCust);
    double accumulatedDistance{0.0};
    constexpr double INF{std::numeric_limits<double>::infinity()};

    for (size_t i{0}; i < numCust; ++i) {
        double minDist{INF};
        size_t bestCandIdx{bestState[0]};

        for (const size_t cIdx : bestState) {
            const double d{dists(i, cIdx)};
            if (d < minDist) {
                minDist = d;
                bestCandIdx = cIdx;
            }
        }

        if (minDist != INF) {
            result.assignments.emplace_back(customers[i].id, candidates[bestCandIdx].id);
            accumulatedDistance += minDist;
        }
    }

    result.finalCost = accumulatedDistance * context.params.costPerKm;
    return result;
}