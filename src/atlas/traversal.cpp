#include "atlas/traversal.hpp"

#include <queue>

namespace atlas {

std::vector<NodeId> breadth_first_order(const Graph& graph, NodeId start) {
    if (!graph.contains(start)) {
        throw GraphError("start node does not exist");
    }
    std::vector<bool> visited(graph.node_count(), false);
    std::queue<NodeId> frontier;
    std::vector<NodeId> order;
    visited[start] = true;
    frontier.push(start);
    while (!frontier.empty()) {
        const NodeId current = frontier.front();
        frontier.pop();
        order.push_back(current);
        for (const Edge& edge : graph.neighbors(current)) {
            if (!visited[edge.to]) {
                visited[edge.to] = true;
                frontier.push(edge.to);
            }
        }
    }
    return order;
}

namespace {

void visit_depth_first(const Graph& graph, NodeId current, std::vector<bool>& visited,
                       std::vector<NodeId>& order) {
    visited[current] = true;
    order.push_back(current);
    for (const Edge& edge : graph.neighbors(current)) {
        if (!visited[edge.to]) {
            visit_depth_first(graph, edge.to, visited, order);
        }
    }
}

}  // namespace

std::vector<NodeId> depth_first_order(const Graph& graph, NodeId start) {
    if (!graph.contains(start)) {
        throw GraphError("start node does not exist");
    }
    std::vector<bool> visited(graph.node_count(), false);
    std::vector<NodeId> order;
    visit_depth_first(graph, start, visited, order);
    return order;
}

}  // namespace atlas
