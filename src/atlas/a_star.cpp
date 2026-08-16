#include "atlas/a_star.hpp"

#include <algorithm>
#include <cmath>
#include <functional>
#include <limits>
#include <queue>
#include <utility>

namespace atlas {

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

    constexpr double infinity = std::numeric_limits<double>::infinity();
    constexpr NodeId no_parent = std::numeric_limits<NodeId>::max();
    std::vector<double> cost(graph.node_count(), infinity);
    std::vector<NodeId> parent(graph.node_count(), no_parent);
    using QueueEntry = std::pair<double, NodeId>;
    std::priority_queue<QueueEntry, std::vector<QueueEntry>, std::greater<>> frontier;
    cost[start] = 0.0;
    frontier.push({euclidean_distance(*start_coordinate, *goal_coordinate), start});

    while (!frontier.empty()) {
        const auto [priority, current] = frontier.top();
        frontier.pop();
        const auto current_coordinate = graph.coordinate(current);
        if (!current_coordinate) {
            throw GraphError("A* requires coordinates for every visited node");
        }
        const double expected_priority =
            cost[current] + euclidean_distance(*current_coordinate, *goal_coordinate);
        if (priority != expected_priority) {
            continue;
        }
        if (current == goal) {
            break;
        }
        for (const Edge& edge : graph.neighbors(current)) {
            const double candidate = cost[current] + edge.weight;
            if (candidate < cost[edge.to]) {
                cost[edge.to] = candidate;
                parent[edge.to] = current;
                const auto next_coordinate = graph.coordinate(edge.to);
                if (!next_coordinate) {
                    throw GraphError("A* requires coordinates for every visited node");
                }
                frontier.push({
                    candidate + euclidean_distance(*next_coordinate, *goal_coordinate),
                    edge.to,
                });
            }
        }
    }

    if (cost[goal] == infinity) {
        return {false, infinity, {}};
    }
    std::vector<NodeId> path;
    for (NodeId current = goal; current != no_parent; current = parent[current]) {
        path.push_back(current);
        if (current == start) {
            break;
        }
    }
    std::reverse(path.begin(), path.end());
    return {true, cost[goal], path};
}

}  // namespace atlas
