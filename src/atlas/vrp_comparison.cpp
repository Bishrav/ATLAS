#include "atlas/vrp_comparison.hpp"

namespace atlas {

VrpComparison compare_nearest_neighbor_and_two_opt(const Graph& graph,
                                                   const VrpProblem& problem) {
    const VrpRoute baseline = nearest_neighbor_route(graph, problem);
    const VrpRoute optimized = two_opt_route(graph, problem);
    const double absolute_improvement = baseline.total_cost - optimized.total_cost;
    const double relative_improvement = baseline.total_cost == 0.0
                                            ? 0.0
                                            : absolute_improvement / baseline.total_cost;
    return {baseline.total_cost, optimized.total_cost, absolute_improvement,
            relative_improvement};
}

}  // namespace atlas
