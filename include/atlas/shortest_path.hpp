#pragma once

#include <vector>

#include "atlas/graph.hpp"

namespace atlas {

struct PathResult {
    bool reachable;
    double cost;
    std::vector<NodeId> nodes;
};

[[nodiscard]] PathResult dijkstra(const Graph& graph, NodeId start, NodeId goal);

}  // namespace atlas
