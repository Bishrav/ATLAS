#pragma once

#include <cstdint>
#include <vector>

#include "atlas/vrp.hpp"

namespace atlas {

struct VrpRoute {
    std::uint32_t vehicle_id;
    std::vector<std::uint32_t> delivery_ids;
    std::vector<NodeId> nodes;
    double total_cost;
};

[[nodiscard]] VrpRoute nearest_neighbor_route(const Graph& graph, const VrpProblem& problem);

}  // namespace atlas
