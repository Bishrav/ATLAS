#pragma once

#include "atlas/shortest_path.hpp"

namespace atlas {

[[nodiscard]] double euclidean_distance(Coordinate from, Coordinate to) noexcept;
[[nodiscard]] PathResult a_star(const Graph& graph, NodeId start, NodeId goal);

}  // namespace atlas
