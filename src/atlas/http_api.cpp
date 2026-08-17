#include "atlas/http_api.hpp"

#include <charconv>
#include <cstdint>
#include <system_error>

namespace atlas {
namespace {

HttpResponse error_response(int status_code, std::string_view code, std::string_view message) {
    return {status_code, "application/json",
            "{\"api_version\":\"" + std::string(kRouteApiVersion) +
                "\",\"error_code\":\"" + std::string(code) + "\",\"message\":\"" +
                std::string(message) + "\"}"};
}

bool parse_node(std::string_view value, NodeId& output) {
    const auto result = std::from_chars(value.data(), value.data() + value.size(), output);
    return result.ec == std::errc{} && result.ptr == value.data() + value.size();
}

bool parse_budget(std::string_view value, std::uint64_t& output) {
    const auto result = std::from_chars(value.data(), value.data() + value.size(), output);
    return result.ec == std::errc{} && result.ptr == value.data() + value.size();
}

RouteAlgorithm parse_algorithm(std::string_view value, bool& valid) {
    valid = true;
    if (value == "dijkstra") return RouteAlgorithm::Dijkstra;
    if (value == "a-star") return RouteAlgorithm::AStar;
    if (value == "bidirectional") return RouteAlgorithm::BidirectionalDijkstra;
    if (value == "alt-a-star") return RouteAlgorithm::AltAStar;
    valid = false;
    return RouteAlgorithm::Dijkstra;
}

}  // namespace

HttpResponse handle_route_http_request(const RoutingService& service, std::string_view method,
                                       std::string_view target) {
    if (method != "GET") {
        return error_response(405, "method_not_allowed", "only GET is supported");
    }
    constexpr std::string_view route_prefix = "/v1/routes?";
    if (!target.starts_with(route_prefix)) {
        return error_response(404, "not_found", "route endpoint does not exist");
    }

    NodeId start = 0;
    NodeId goal = 0;
    std::uint64_t budget = 0;
    std::string_view algorithm_value;
    bool has_start = false;
    bool has_goal = false;
    bool has_algorithm = false;
    std::string_view query = target.substr(route_prefix.size());
    while (!query.empty()) {
        const std::size_t separator = query.find('&');
        const std::string_view item = query.substr(0, separator);
        const std::size_t equals = item.find('=');
        if (equals == std::string_view::npos) {
            return error_response(400, "invalid_query", "query parameters require name=value");
        }
        const std::string_view name = item.substr(0, equals);
        const std::string_view value = item.substr(equals + 1);
        if (name == "start") {
            has_start = parse_node(value, start);
        } else if (name == "goal") {
            has_goal = parse_node(value, goal);
        } else if (name == "algorithm") {
            algorithm_value = value;
            has_algorithm = true;
        } else if (name == "max_nodes_expanded") {
            if (!parse_budget(value, budget)) {
                return error_response(400, "invalid_query", "max_nodes_expanded must be numeric");
            }
        } else {
            return error_response(400, "invalid_query", "unknown query parameter");
        }
        if (separator == std::string_view::npos) break;
        query.remove_prefix(separator + 1);
    }
    if (!has_start || !has_goal || !has_algorithm) {
        return error_response(400, "invalid_query", "start, goal, and algorithm are required");
    }
    bool algorithm_valid = false;
    const RouteAlgorithm algorithm = parse_algorithm(algorithm_value, algorithm_valid);
    if (!algorithm_valid) {
        return error_response(400, "invalid_algorithm", "unsupported route algorithm");
    }
    try {
        const RouteResponse response = service.route({start, goal, algorithm, budget});
        return {200, "application/json", serialize_route_response(response)};
    } catch (const RoutingServiceError& error) {
        return error_response(422, "route_request_failed", error.what());
    } catch (const GraphError& error) {
        return error_response(422, "route_request_failed", error.what());
    }
}

}  // namespace atlas
