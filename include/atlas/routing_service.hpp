#pragma once

#include <cstdint>
#include <stdexcept>

#include "atlas/a_star.hpp"
#include "atlas/bidirectional.hpp"

namespace atlas {

inline constexpr const char* kRouteApiVersion = "ATLAS_ROUTE_API_V1";

enum class RouteAlgorithm {
    Dijkstra,
    AStar,
    BidirectionalDijkstra,
    AltAStar,
};

struct RouteRequest {
    NodeId start;
    NodeId goal;
    RouteAlgorithm algorithm;
};

struct RouteResponse {
    PathResult route;
    std::uint64_t graph_revision;
};

class RoutingServiceError : public std::invalid_argument {
public:
    using std::invalid_argument::invalid_argument;
};

class RoutingService {
public:
    RoutingService(const Graph& graph, const LandmarkIndex* landmarks = nullptr) noexcept;

    [[nodiscard]] RouteResponse route(const RouteRequest& request) const;

private:
    const Graph& graph_;
    const LandmarkIndex* landmarks_;
};

}  // namespace atlas
