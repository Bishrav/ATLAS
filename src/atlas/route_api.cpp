#include "atlas/route_api.hpp"

#include <cmath>
#include <iomanip>
#include <sstream>

namespace atlas {

std::string serialize_route_response(const RouteResponse& response) {
    std::ostringstream output;
    output << std::setprecision(17);
    output << "{\"api_version\":\"" << kRouteApiVersion << "\",\"graph_revision\":"
           << response.graph_revision << ",\"reachable\":"
           << (response.route.reachable ? "true" : "false") << ",\"cost\":";
    if (response.route.reachable && std::isfinite(response.route.cost)) {
        output << response.route.cost;
    } else {
        output << "null";
    }
    output << ",\"nodes\":[";
    for (std::size_t index = 0; index < response.route.nodes.size(); ++index) {
        if (index != 0) {
            output << ',';
        }
        output << response.route.nodes[index];
    }
    output << "],\"metrics\":{\"nodes_expanded\":"
           << response.route.metrics.nodes_expanded << ",\"queue_pushes\":"
           << response.route.metrics.queue_pushes << "}}";
    return output.str();
}

}  // namespace atlas
