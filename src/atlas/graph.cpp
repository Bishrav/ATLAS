#include "atlas/graph.hpp"

#include <cmath>

namespace atlas {

Graph::Graph(std::size_t node_count) : adjacency_(node_count) {}

void Graph::add_node(NodeId id) {
    if (static_cast<std::size_t>(id) < adjacency_.size()) {
        throw GraphError("node already exists");
    }
    adjacency_.resize(static_cast<std::size_t>(id) + 1);
}

void Graph::add_edge(NodeId from, NodeId to, double weight) {
    if (!contains(from) || !contains(to)) {
        throw GraphError("edge endpoint does not exist");
    }
    if (!std::isfinite(weight) || weight < 0.0) {
        throw GraphError("edge weight must be finite and non-negative");
    }

    const auto& edges = adjacency_[from];
    for (const Edge& edge : edges) {
        if (edge.to == to) {
            throw GraphError("duplicate directed edge");
        }
    }
    adjacency_[from].push_back({to, weight});
    ++edge_count_;
}

bool Graph::contains(NodeId id) const noexcept {
    return static_cast<std::size_t>(id) < adjacency_.size();
}

std::size_t Graph::node_count() const noexcept {
    return adjacency_.size();
}

std::size_t Graph::edge_count() const noexcept {
    return edge_count_;
}

const std::vector<Edge>& Graph::neighbors(NodeId from) const {
    if (!contains(from)) {
        throw GraphError("node does not exist");
    }
    return adjacency_[from];
}

}  // namespace atlas
