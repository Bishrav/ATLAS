#include "atlas/vrp_evaluation.hpp"

namespace atlas {

VrpEvaluation evaluate_nearest_neighbor(const Graph& graph, const VrpProblem& problem) {
    const VrpRoute route = nearest_neighbor_route(graph, problem);
    double total_demand = 0.0;
    for (const Delivery& delivery : problem.deliveries) {
        total_demand += delivery.demand;
    }
    return {"nearest_neighbor",
            problem.deliveries.size(),
            route.delivery_ids.size(),
            route.nodes.size(),
            route.total_cost,
            total_demand / problem.vehicles.front().capacity};
}

}  // namespace atlas
