#pragma once

#include "atlas/shortest_path.hpp"

namespace atlas {

[[nodiscard]] PathResult bidirectional_dijkstra(const Graph& graph, NodeId start, NodeId goal);

}  // namespace atlas
