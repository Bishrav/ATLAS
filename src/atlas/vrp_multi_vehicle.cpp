#include "atlas/vrp_multi_vehicle.hpp"

#include <cstddef>
#include <utility>
#include <vector>

namespace atlas {

MultiVehicleSolution multi_vehicle_nearest_neighbor(const Graph& graph,
                                                    const VrpProblem& problem) {
    validate_vrp_problem(graph, problem);
    for (const Delivery& delivery : problem.deliveries) {
        if (delivery.time_window) {
            throw VrpError("multi-vehicle baseline does not schedule time windows");
        }
    }

    std::vector<std::vector<Delivery>> assignments(problem.vehicles.size());
    std::vector<double> remaining_capacity;
    remaining_capacity.reserve(problem.vehicles.size());
    for (const Vehicle& vehicle : problem.vehicles) {
        remaining_capacity.push_back(vehicle.capacity);
    }
    for (const Delivery& delivery : problem.deliveries) {
        std::size_t selected_vehicle = problem.vehicles.size();
        for (std::size_t index = 0; index < remaining_capacity.size(); ++index) {
            if (delivery.demand <= remaining_capacity[index]) {
                selected_vehicle = index;
                break;
            }
        }
        if (selected_vehicle == problem.vehicles.size()) {
            throw VrpError("delivery cannot be assigned within vehicle capacities");
        }
        assignments[selected_vehicle].push_back(delivery);
        remaining_capacity[selected_vehicle] -= delivery.demand;
    }

    MultiVehicleSolution solution;
    solution.total_cost = 0.0;
    solution.routes.reserve(problem.vehicles.size());
    for (std::size_t index = 0; index < problem.vehicles.size(); ++index) {
        const VrpProblem vehicle_problem{problem.depot, assignments[index],
                                         {problem.vehicles[index]}};
        VrpRoute route = nearest_neighbor_route(graph, vehicle_problem);
        solution.total_cost += route.total_cost;
        solution.routes.push_back(std::move(route));
    }
    return solution;
}

}  // namespace atlas
