#pragma once

#include "atlas/vrp_baseline.hpp"

namespace atlas {

[[nodiscard]] VrpRoute two_opt_route(const Graph& graph, const VrpProblem& problem);

}  // namespace atlas
