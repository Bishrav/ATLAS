#pragma once

#include <string>
#include <string_view>

#include "atlas/route_api.hpp"

namespace atlas {

struct HttpResponse {
    int status_code;
    std::string content_type;
    std::string body;
};

[[nodiscard]] HttpResponse handle_route_http_request(
    const RoutingService& service, std::string_view method, std::string_view target);

}  // namespace atlas
