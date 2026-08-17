#pragma once

#include <vector>

#include "atlas/vrp_baseline.hpp"

namespace atlas {

struct MultiVehicleSolution {
    std::vector<VrpRoute> routes;
    double total_cost;
};

[[nodiscard]] MultiVehicleSolution multi_vehicle_nearest_neighbor(
    const Graph& graph, const VrpProblem& problem);

}  // namespace atlas
