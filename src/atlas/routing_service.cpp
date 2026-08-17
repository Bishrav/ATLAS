#include "atlas/routing_service.hpp"

namespace atlas {

RoutingService::RoutingService(const Graph& graph, const LandmarkIndex* landmarks) noexcept
    : graph_(graph), landmarks_(landmarks) {}

RouteResponse RoutingService::route(const RouteRequest& request) const {
    ++metrics_.requests;
    try {
        if (!graph_.contains(request.start) || !graph_.contains(request.goal)) {
            throw RoutingServiceError("route endpoint does not exist");
        }

        RouteResponse response;
        switch (request.algorithm) {
        case RouteAlgorithm::Dijkstra:
            response = {dijkstra(graph_, request.start, request.goal), graph_.revision()};
            break;
        case RouteAlgorithm::AStar:
            response = {a_star(graph_, request.start, request.goal), graph_.revision()};
            break;
        case RouteAlgorithm::BidirectionalDijkstra:
            response = {bidirectional_dijkstra(graph_, request.start, request.goal),
                        graph_.revision()};
            break;
        case RouteAlgorithm::AltAStar:
            if (landmarks_ == nullptr) {
                throw RoutingServiceError("ALT routing requires a landmark index");
            }
            response = {a_star(graph_, request.start, request.goal, *landmarks_),
                        graph_.revision()};
            break;
        default:
            throw RoutingServiceError("unsupported route algorithm");
        }
        if (request.max_nodes_expanded != 0 &&
            response.route.metrics.nodes_expanded > request.max_nodes_expanded) {
            throw RoutingServiceError("route search budget exceeded");
        }
        ++metrics_.successful_requests;
        return response;
    } catch (...) {
        ++metrics_.failed_requests;
        throw;
    }
}

RoutingServiceMetrics RoutingService::metrics() const noexcept {
    return metrics_;
}

}  // namespace atlas
