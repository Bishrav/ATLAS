#include "atlas/a_star.hpp"

#include <algorithm>
#include <cmath>
#include <functional>
#include <limits>
#include <queue>
#include <utility>

namespace atlas {
namespace {

template <typename Heuristic>
PathResult a_star_search(const Graph& graph, NodeId start, NodeId goal, Heuristic heuristic) {
    if (!graph.contains(start) || !graph.contains(goal)) {
        throw GraphError("path endpoint does not exist");
    }

    constexpr double infinity = std::numeric_limits<double>::infinity();
    constexpr NodeId no_parent = std::numeric_limits<NodeId>::max();
    std::vector<double> cost(graph.node_count(), infinity);
    std::vector<NodeId> parent(graph.node_count(), no_parent);
    using QueueEntry = std::pair<double, NodeId>;
    std::priority_queue<QueueEntry, std::vector<QueueEntry>, std::greater<>> frontier;
    cost[start] = 0.0;
    frontier.push({heuristic(start), start});
    SearchMetrics metrics;
    ++metrics.queue_pushes;

    while (!frontier.empty()) {
        const auto [priority, current] = frontier.top();
        frontier.pop();
        const double expected_priority = cost[current] + heuristic(current);
        if (priority != expected_priority) {
            continue;
        }
        ++metrics.nodes_expanded;
        if (current == goal) {
            break;
        }
        for (const Edge& edge : graph.neighbors(current)) {
            const double candidate = cost[current] + edge.weight;
            if (candidate < cost[edge.to]) {
                cost[edge.to] = candidate;
                parent[edge.to] = current;
                frontier.push({candidate + heuristic(edge.to), edge.to});
                ++metrics.queue_pushes;
            }
        }
    }

    if (cost[goal] == infinity) {
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
    return {true, cost[goal], path, metrics};
}

}  // namespace

double euclidean_distance(Coordinate from, Coordinate to) noexcept {
    const double x = from.x - to.x;
    const double y = from.y - to.y;
    return std::sqrt(x * x + y * y);
}

PathResult a_star(const Graph& graph, NodeId start, NodeId goal) {
    if (!graph.contains(start) || !graph.contains(goal)) {
        throw GraphError("path endpoint does not exist");
    }
    const auto start_coordinate = graph.coordinate(start);
    const auto goal_coordinate = graph.coordinate(goal);
    if (!start_coordinate || !goal_coordinate) {
        throw GraphError("A* requires coordinates for path endpoints");
    }

    return a_star_search(graph, start, goal, [&](NodeId node) {
        const auto coordinate = graph.coordinate(node);
        if (!coordinate) {
            throw GraphError("A* requires coordinates for every visited node");
        }
        return euclidean_distance(*coordinate, *goal_coordinate);
    });
}

PathResult a_star(const Graph& graph, NodeId start, NodeId goal,
                  const LandmarkIndex& landmarks) {
    if (!landmarks.matches_revision(graph.revision())) {
        throw GraphError("ALT landmark index does not match graph revision");
    }
    return a_star_search(graph, start, goal,
                         [&](NodeId node) { return landmarks.heuristic(node, goal); });
}

}  // namespace atlas
