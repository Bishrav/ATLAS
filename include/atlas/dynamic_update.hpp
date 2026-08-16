#pragma once

#include <vector>

#include "atlas/graph.hpp"

namespace atlas {

enum class UpdateKind {
    ChangeWeight,
    CloseEdge,
    OpenEdge,
};

struct GraphUpdate {
    UpdateKind kind;
    NodeId from;
    NodeId to;
    double weight = 0.0;
};

void apply_update(Graph& graph, const GraphUpdate& update);
void replay_updates(Graph& graph, const std::vector<GraphUpdate>& updates);

}  // namespace atlas
