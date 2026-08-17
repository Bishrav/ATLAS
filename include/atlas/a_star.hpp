#pragma once

#include "atlas/shortest_path.hpp"
#include "atlas/landmark.hpp"

namespace atlas {

[[nodiscard]] double euclidean_distance(Coordinate from, Coordinate to) noexcept;
[[nodiscard]] PathResult a_star(const Graph& graph, NodeId start, NodeId goal);
[[nodiscard]] PathResult a_star(const Graph& graph, NodeId start, NodeId goal,
                                const LandmarkIndex& landmarks);

}  // namespace atlas
