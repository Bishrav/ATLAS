#include "atlas/shortest_path.hpp"

#include <algorithm>
#include <functional>
#include <limits>
#include <queue>
#include <utility>

namespace atlas {

PathResult dijkstra(const Graph& graph, NodeId start, NodeId goal) {
    if (!graph.contains(start) || !graph.contains(goal)) {
        throw GraphError("path endpoint does not exist");
    }

    constexpr double infinity = std::numeric_limits<double>::infinity();
    constexpr NodeId no_parent = std::numeric_limits<NodeId>::max();
    std::vector<double> distance(graph.node_count(), infinity);
    std::vector<NodeId> parent(graph.node_count(), no_parent);
    using QueueEntry = std::pair<double, NodeId>;
    std::priority_queue<QueueEntry, std::vector<QueueEntry>, std::greater<>> frontier;

    distance[start] = 0.0;
    frontier.push({0.0, start});
    SearchMetrics metrics;
    ++metrics.queue_pushes;
    while (!frontier.empty()) {
        const auto [current_distance, current] = frontier.top();
        frontier.pop();
        if (current_distance != distance[current]) {
            continue;
        }
        ++metrics.nodes_expanded;
        if (current == goal) {
            break;
        }
        for (const Edge& edge : graph.neighbors(current)) {
            const double candidate = current_distance + edge.weight;
            if (candidate < distance[edge.to]) {
                distance[edge.to] = candidate;
                parent[edge.to] = current;
                frontier.push({candidate, edge.to});
                ++metrics.queue_pushes;
            }
        }
    }

    if (distance[goal] == infinity) {
        return {false, infinity, {}, metrics};
    }

    std::vector<NodeId> path;
    for (NodeId current = goal; current != no_parent; current = parent[current]) {
        path.push_back(current);
        if (current == start) {
            break;
        }
    }
    std::reverse(path.begin(), path.end());
    return {true, distance[goal], path, metrics};
}

}  // namespace atlas
