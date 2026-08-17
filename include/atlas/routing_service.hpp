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
    std::uint64_t max_nodes_expanded = 0;
};

struct RouteResponse {
    PathResult route;
    std::uint64_t graph_revision;
};

struct RoutingServiceMetrics {
    std::uint64_t requests = 0;
    std::uint64_t successful_requests = 0;
    std::uint64_t failed_requests = 0;
};

class RoutingServiceError : public std::invalid_argument {
public:
    using std::invalid_argument::invalid_argument;
};

class RoutingService {
public:
    RoutingService(const Graph& graph, const LandmarkIndex* landmarks = nullptr) noexcept;

    [[nodiscard]] RouteResponse route(const RouteRequest& request) const;
    [[nodiscard]] RoutingServiceMetrics metrics() const noexcept;

private:
    const Graph& graph_;
    const LandmarkIndex* landmarks_;
    mutable RoutingServiceMetrics metrics_;
};

}  // namespace atlas
