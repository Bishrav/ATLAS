#include "atlas/dynamic_update.hpp"

namespace atlas {

void apply_update(Graph& graph, const GraphUpdate& update) {
    switch (update.kind) {
    case UpdateKind::ChangeWeight:
        graph.update_edge_weight(update.from, update.to, update.weight);
        return;
    case UpdateKind::CloseEdge:
        graph.remove_edge(update.from, update.to);
        return;
    case UpdateKind::OpenEdge:
        graph.add_edge(update.from, update.to, update.weight);
        return;
    }
    throw GraphError("unsupported graph update kind");
}

void replay_updates(Graph& graph, const std::vector<GraphUpdate>& updates) {
    for (const GraphUpdate& update : updates) {
        apply_update(graph, update);
    }
}

}  // namespace atlas
