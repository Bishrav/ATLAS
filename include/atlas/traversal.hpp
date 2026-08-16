#pragma once

#include <vector>

#include "atlas/graph.hpp"

namespace atlas {

[[nodiscard]] std::vector<NodeId> breadth_first_order(const Graph& graph, NodeId start);
[[nodiscard]] std::vector<NodeId> depth_first_order(const Graph& graph, NodeId start);

}  // namespace atlas
