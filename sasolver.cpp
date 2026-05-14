#include "SASolver.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <queue>
#include <random>

SolverResult SASolver::solve(const SolverContext &context)
{
    const auto &customers = context.customers;
    const auto &candidates = context.candidates;
    const auto &params = context.params;
    const auto &adjList = context.adjList;

    if (customers.empty() || candidates.empty() || params.pMedians <= 0) {
        return SolverResult{};
    }

    const size_t numCust = customers.size();
    const size_t numCand = candidates.size();
    const size_t p = std::min(static_cast<size_t>(params.pMedians), numCand);

    constexpr double INF = std::numeric_limits<double>::infinity();
    std::vector<std::vector<double>> distMatrix(numCust, std::vector<double>(numCand, INF));

    using PQElement = std::pair<double, NodeId>;

    for (size_t j{0}; j < numCand; ++j) {
        std::unordered_map<NodeId, double> distances;
        std::priority_queue<PQElement, std::vector<PQElement>, std::greater<>> pq;

        const NodeId startNode = candidates[j].id;
        distances[startNode] = 0.0;
        pq.push({0.0, startNode});

        while (!pq.empty()) {
            const auto [currDist, currNode] = pq.top();
            pq.pop();

            if (currDist > distances[currNode]) {
                continue;
            }

            if (auto it = adjList.find(currNode); it != adjList.end()) {
                for (const auto &edge : it->second) {
                    const double newDist = currDist + edge.length;
                    if (!distances.contains(edge.to) || newDist < distances[edge.to]) {
                        distances[edge.to] = newDist;
                        pq.push({newDist, edge.to});
                    }
                }
            }
        }

        for (size_t i{0}; i < numCust; ++i) {
            if (distances.contains(customers[i].id)) {
                distMatrix[i][j] = distances[customers[i].id];
            }
        }
    }

    std::vector<size_t> bestState(p);
    std::iota(bestState.begin(), bestState.end(), 0);

    if (p < numCand) {
        auto calculateCost = [&](const std::vector<size_t> &state) -> double {
            double totalCost{0.0};
            for (size_t i{0}; i < numCust; ++i) {
                double minDist = INF;
                for (const size_t cIdx : state) {
                    minDist = std::min(minDist, distMatrix[i][cIdx]);
                }
                totalCost += minDist;
            }
            return totalCost * params.costPerKm;
        };

        std::mt19937 rng{std::random_device{}()};
        std::vector<size_t> currentState = bestState;
        double currentCost = calculateCost(currentState);
        double bestCost = currentCost;

        double temp = params.tMax;
        std::uniform_real_distribution<double> chanceDist(0.0, 1.0);

        while (temp > params.tMin) {
            for (int step{0}; step < params.iterationsPerStep; ++step) {
                std::vector<size_t> neighborState = currentState;

                std::uniform_int_distribution<size_t> activeDist(0, p - 1);
                const size_t swapIdx = activeDist(rng);

                size_t newCandIdx;
                do {
                    std::uniform_int_distribution<size_t> allDist(0, numCand - 1);
                    newCandIdx = allDist(rng);
                } while (std::ranges::find(neighborState, newCandIdx) != neighborState.end());

                neighborState[swapIdx] = newCandIdx;

                const double neighborCost = calculateCost(neighborState);
                const double delta = neighborCost - currentCost;

                if (delta < 0.0 || chanceDist(rng) < std::exp(-delta / temp)) {
                    currentState = std::move(neighborState);
                    currentCost = neighborCost;

                    if (currentCost < bestCost) {
                        bestState = currentState;
                        bestCost = currentCost;
                    }
                }
            }
            temp *= params.coolingFactor;
        }
    }

    SolverResult result;
    result.chosenCandidates.reserve(p);
    for (const size_t idx : bestState) {
        result.chosenCandidates.push_back(candidates[idx].id);
    }

    result.assignments.reserve(numCust);
    for (size_t i{0}; i < numCust; ++i) {
        double minDist = INF;
        size_t bestCandIdx = bestState[0];

        for (const size_t cIdx : bestState) {
            if (distMatrix[i][cIdx] < minDist) {
                minDist = distMatrix[i][cIdx];
                bestCandIdx = cIdx;
            }
        }

        if (minDist != INF) {
            result.assignments.emplace_back(customers[i].id, candidates[bestCandIdx].id);
            result.finalCost += minDist;
        }
    }

    result.finalCost *= params.costPerKm;

    return result;
}