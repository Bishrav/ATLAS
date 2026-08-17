#pragma once

#include <string>

#include "atlas/routing_service.hpp"

namespace atlas {

[[nodiscard]] std::string serialize_route_response(const RouteResponse& response);

}  // namespace atlas
