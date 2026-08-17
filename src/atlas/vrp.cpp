#include "atlas/vrp.hpp"

#include <cmath>
#include <unordered_set>

namespace atlas {

void validate_vrp_problem(const Graph& graph, const VrpProblem& problem) {
    if (!graph.contains(problem.depot)) {
        throw VrpError("VRP depot does not exist");
    }
    if (problem.vehicles.empty()) {
        throw VrpError("VRP requires at least one vehicle");
    }

    std::unordered_set<std::uint32_t> delivery_ids;
    for (const Delivery& delivery : problem.deliveries) {
        if (!delivery_ids.insert(delivery.id).second) {
            throw VrpError("delivery IDs must be unique");
        }
        if (!graph.contains(delivery.location)) {
            throw VrpError("delivery location does not exist");
        }
        if (!std::isfinite(delivery.demand) || delivery.demand <= 0.0) {
            throw VrpError("delivery demand must be finite and positive");
        }
        if (delivery.time_window &&
            (!std::isfinite(delivery.time_window->earliest) ||
             !std::isfinite(delivery.time_window->latest) ||
             delivery.time_window->earliest > delivery.time_window->latest)) {
            throw VrpError("delivery time window is invalid");
        }
    }

    std::unordered_set<std::uint32_t> vehicle_ids;
    for (const Vehicle& vehicle : problem.vehicles) {
        if (!vehicle_ids.insert(vehicle.id).second) {
            throw VrpError("vehicle IDs must be unique");
        }
        if (!graph.contains(vehicle.start) ||
            (vehicle.end && !graph.contains(*vehicle.end))) {
            throw VrpError("vehicle location does not exist");
        }
        if (!std::isfinite(vehicle.capacity) || vehicle.capacity <= 0.0) {
            throw VrpError("vehicle capacity must be finite and positive");
        }
    }
}

}  // namespace atlas
