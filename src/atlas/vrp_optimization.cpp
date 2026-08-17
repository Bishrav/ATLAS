#include "atlas/vrp_optimization.hpp"

#include <algorithm>
#include <cstddef>
#include <unordered_map>
#include <utility>

#include "atlas/shortest_path.hpp"

namespace atlas {
namespace {

VrpRoute route_for_order(const Graph& graph, const VrpProblem& problem,
                         const std::vector<std::uint32_t>& order) {
    const Vehicle& vehicle = problem.vehicles.front();
    std::unordered_map<std::uint32_t, const Delivery*> deliveries;
    for (const Delivery& delivery : problem.deliveries) {
        deliveries.emplace(delivery.id, &delivery);
    }

    NodeId current = vehicle.start;
    double total_cost = 0.0;
    std::vector<NodeId> nodes{current};
    for (const std::uint32_t delivery_id : order) {
        const PathResult leg = dijkstra(graph, current, deliveries.at(delivery_id)->location);
        if (!leg.reachable) {
            throw VrpError("delivery is unreachable from current route");
        }
        total_cost += leg.cost;
        nodes.insert(nodes.end(), leg.nodes.begin() + 1, leg.nodes.end());
        current = deliveries.at(delivery_id)->location;
    }

    const PathResult finish = dijkstra(graph, current, vehicle.end.value_or(problem.depot));
    if (!finish.reachable) {
        throw VrpError("route cannot reach its final location");
    }
    total_cost += finish.cost;
    nodes.insert(nodes.end(), finish.nodes.begin() + 1, finish.nodes.end());
    return {vehicle.id, order, std::move(nodes), total_cost};
}

}  // namespace

VrpRoute two_opt_route(const Graph& graph, const VrpProblem& problem) {
    VrpRoute best = nearest_neighbor_route(graph, problem);
    if (best.delivery_ids.size() < 2) {
        return best;
    }

    bool improved = true;
    while (improved) {
        improved = false;
        for (std::size_t first = 0; first + 1 < best.delivery_ids.size() && !improved; ++first) {
            for (std::size_t last = first + 1; last < best.delivery_ids.size(); ++last) {
                auto candidate_order = best.delivery_ids;
                std::reverse(candidate_order.begin() + static_cast<std::ptrdiff_t>(first),
                             candidate_order.begin() + static_cast<std::ptrdiff_t>(last + 1));
                const VrpRoute candidate = route_for_order(graph, problem, candidate_order);
                if (candidate.total_cost < best.total_cost - 1e-9) {
                    best = candidate;
                    improved = true;
                    break;
                }
            }
        }
    }
    return best;
}

}  // namespace atlas
