#pragma once

#include <cstdint>
#include <optional>
#include <stdexcept>
#include <vector>

#include "atlas/graph.hpp"

namespace atlas {

inline constexpr const char* kVrpProblemVersion = "ATLAS_VRP_PROBLEM_V1";

class VrpError : public std::invalid_argument {
public:
    using std::invalid_argument::invalid_argument;
};

struct TimeWindow {
    double earliest;
    double latest;
};

struct Delivery {
    std::uint32_t id;
    NodeId location;
    double demand;
    std::optional<TimeWindow> time_window;
};

struct Vehicle {
    std::uint32_t id;
    NodeId start;
    std::optional<NodeId> end;
    double capacity;
};

struct VrpProblem {
    NodeId depot;
    std::vector<Delivery> deliveries;
    std::vector<Vehicle> vehicles;
};

void validate_vrp_problem(const Graph& graph, const VrpProblem& problem);

}  // namespace atlas
