#include "atlas/bidirectional.hpp"

#include <algorithm>
#include <functional>
#include <limits>
#include <queue>
#include <utility>

namespace atlas {

PathResult bidirectional_dijkstra(const Graph& graph, NodeId start, NodeId goal) {
    if (!graph.contains(start) || !graph.contains(goal)) {
        throw GraphError("path endpoint does not exist");
    }
    if (start == goal) {
        return {true, 0.0, {start}};
    }

    constexpr double infinity = std::numeric_limits<double>::infinity();
    constexpr NodeId no_parent = std::numeric_limits<NodeId>::max();
    using QueueEntry = std::pair<double, NodeId>;
    using Queue = std::priority_queue<QueueEntry, std::vector<QueueEntry>, std::greater<>>;
    std::vector<std::vector<Edge>> reverse_edges(graph.node_count());
    for (NodeId from = 0; from < graph.node_count(); ++from) {
        for (const Edge& edge : graph.neighbors(from)) {
            reverse_edges[edge.to].push_back({from, edge.weight});
        }
    }

    std::vector<double> forward_distance(graph.node_count(), infinity);
    std::vector<double> backward_distance(graph.node_count(), infinity);
    std::vector<NodeId> forward_parent(graph.node_count(), no_parent);
    std::vector<NodeId> backward_next(graph.node_count(), no_parent);
    Queue forward_frontier;
    Queue backward_frontier;
    forward_distance[start] = 0.0;
    backward_distance[goal] = 0.0;
    forward_frontier.push({0.0, start});
    backward_frontier.push({0.0, goal});
    SearchMetrics metrics;
    metrics.queue_pushes = 2;
    double best_cost = infinity;
    NodeId meeting = no_parent;

    auto consider = [&](NodeId node) {
        if (forward_distance[node] == infinity || backward_distance[node] == infinity) {
            return;
        }
        const double candidate = forward_distance[node] + backward_distance[node];
        if (candidate < best_cost || (candidate == best_cost && node < meeting)) {
            best_cost = candidate;
            meeting = node;
        }
    };

    while (!forward_frontier.empty() || !backward_frontier.empty()) {
        if (!forward_frontier.empty()) {
            const auto [distance, current] = forward_frontier.top();
            forward_frontier.pop();
            if (distance == forward_distance[current]) {
                ++metrics.nodes_expanded;
                consider(current);
                for (const Edge& edge : graph.neighbors(current)) {
                    const double candidate = distance + edge.weight;
                    if (candidate < forward_distance[edge.to]) {
                        forward_distance[edge.to] = candidate;
                        forward_parent[edge.to] = current;
                        forward_frontier.push({candidate, edge.to});
                        ++metrics.queue_pushes;
                    }
                }
            }
        }
        if (!backward_frontier.empty()) {
            const auto [distance, current] = backward_frontier.top();
            backward_frontier.pop();
            if (distance == backward_distance[current]) {
                ++metrics.nodes_expanded;
                consider(current);
                for (const Edge& edge : reverse_edges[current]) {
                    const double candidate = distance + edge.weight;
                    if (candidate < backward_distance[edge.to]) {
                        backward_distance[edge.to] = candidate;
                        backward_next[edge.to] = current;
                        backward_frontier.push({candidate, edge.to});
                        ++metrics.queue_pushes;
                    }
                }
            }
        }
    }

    if (meeting == no_parent) {
        return {false, infinity, {}, metrics};
    }

    std::vector<NodeId> path;
    for (NodeId current = meeting; current != no_parent; current = forward_parent[current]) {
        path.push_back(current);
        if (current == start) {
            break;
        }
    }
    std::reverse(path.begin(), path.end());
    for (NodeId current = meeting; current != goal; current = backward_next[current]) {
        if (current == no_parent) {
            return {false, infinity, {}, metrics};
        }
        path.push_back(backward_next[current]);
    }
    return {true, best_cost, path, metrics};
}

}  // namespace atlas
