#include "atlas/vrp_baseline.hpp"

#include <algorithm>
#include <limits>
#include <utility>

#include "atlas/shortest_path.hpp"

namespace atlas {

VrpRoute nearest_neighbor_route(const Graph& graph, const VrpProblem& problem) {
    validate_vrp_problem(graph, problem);
    if (problem.vehicles.size() != 1) {
        throw VrpError("nearest-neighbor baseline currently supports one vehicle");
    }
    for (const Delivery& delivery : problem.deliveries) {
        if (delivery.time_window) {
            throw VrpError("nearest-neighbor baseline does not schedule time windows");
        }
    }

    const Vehicle& vehicle = problem.vehicles.front();
    double demand = 0.0;
    for (const Delivery& delivery : problem.deliveries) {
        demand += delivery.demand;
    }
    if (demand > vehicle.capacity) {
        throw VrpError("delivery demand exceeds vehicle capacity");
    }

    std::vector<bool> visited(problem.deliveries.size(), false);
    std::vector<std::uint32_t> delivery_ids;
    std::vector<NodeId> route_nodes{vehicle.start};
    NodeId current = vehicle.start;
    double total_cost = 0.0;
    for (std::size_t visit = 0; visit < problem.deliveries.size(); ++visit) {
        std::size_t selected = problem.deliveries.size();
        PathResult selected_path{false, std::numeric_limits<double>::infinity(), {}, {}};
        for (std::size_t index = 0; index < problem.deliveries.size(); ++index) {
            if (visited[index]) {
                continue;
            }
            const PathResult candidate =
                dijkstra(graph, current, problem.deliveries[index].location);
            if (!candidate.reachable) {
                continue;
            }
            if (!selected_path.reachable || candidate.cost < selected_path.cost ||
                (candidate.cost == selected_path.cost &&
                 problem.deliveries[index].id < problem.deliveries[selected].id)) {
                selected = index;
                selected_path = candidate;
            }
        }
        if (selected == problem.deliveries.size()) {
            throw VrpError("delivery is unreachable from current route");
        }
        visited[selected] = true;
        delivery_ids.push_back(problem.deliveries[selected].id);
        total_cost += selected_path.cost;
        route_nodes.insert(route_nodes.end(), selected_path.nodes.begin() + 1,
                           selected_path.nodes.end());
        current = problem.deliveries[selected].location;
    }

    const NodeId finish = vehicle.end.value_or(problem.depot);
    const PathResult return_path = dijkstra(graph, current, finish);
    if (!return_path.reachable) {
        throw VrpError("route cannot reach its final location");
    }
    total_cost += return_path.cost;
    route_nodes.insert(route_nodes.end(), return_path.nodes.begin() + 1, return_path.nodes.end());
    return {vehicle.id, std::move(delivery_ids), std::move(route_nodes), total_cost};
}

}  // namespace atlas
