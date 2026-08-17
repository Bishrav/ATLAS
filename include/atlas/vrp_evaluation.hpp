#pragma once

#include <cstddef>
#include <string>

#include "atlas/vrp_baseline.hpp"

namespace atlas {

struct VrpEvaluation {
    std::string algorithm;
    std::size_t requested_deliveries;
    std::size_t delivered_deliveries;
    std::size_t route_node_count;
    double total_cost;
    double capacity_utilization;
};

[[nodiscard]] VrpEvaluation evaluate_nearest_neighbor(
    const Graph& graph, const VrpProblem& problem);

}  // namespace atlas
