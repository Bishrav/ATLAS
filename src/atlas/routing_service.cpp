#include "atlas/routing_service.hpp"

namespace atlas {

RoutingService::RoutingService(const Graph& graph, const LandmarkIndex* landmarks) noexcept
    : graph_(graph), landmarks_(landmarks) {}

RouteResponse RoutingService::route(const RouteRequest& request) const {
    if (!graph_.contains(request.start) || !graph_.contains(request.goal)) {
        throw RoutingServiceError("route endpoint does not exist");
    }

    switch (request.algorithm) {
    case RouteAlgorithm::Dijkstra:
        return {dijkstra(graph_, request.start, request.goal), graph_.revision()};
    case RouteAlgorithm::AStar:
        return {a_star(graph_, request.start, request.goal), graph_.revision()};
    case RouteAlgorithm::BidirectionalDijkstra:
        return {bidirectional_dijkstra(graph_, request.start, request.goal), graph_.revision()};
    case RouteAlgorithm::AltAStar:
        if (landmarks_ == nullptr) {
            throw RoutingServiceError("ALT routing requires a landmark index");
        }
        return {a_star(graph_, request.start, request.goal, *landmarks_), graph_.revision()};
    }
    throw RoutingServiceError("unsupported route algorithm");
}

}  // namespace atlas
