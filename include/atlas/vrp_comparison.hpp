#pragma once

#include "atlas/vrp_optimization.hpp"

namespace atlas {

struct VrpComparison {
    double baseline_cost;
    double optimized_cost;
    double absolute_improvement;
    double relative_improvement;
};

[[nodiscard]] VrpComparison compare_nearest_neighbor_and_two_opt(
    const Graph& graph, const VrpProblem& problem);

}  // namespace atlas
