#pragma once

#include <vector>

#include "atlas/graph.hpp"

namespace atlas {

struct SearchMetrics {
    std::size_t nodes_expanded = 0;
    std::size_t queue_pushes = 0;
};

struct PathResult {
    bool reachable;
    double cost;
    std::vector<NodeId> nodes;
    SearchMetrics metrics;
};

[[nodiscard]] PathResult dijkstra(const Graph& graph, NodeId start, NodeId goal);

}  // namespace atlas
