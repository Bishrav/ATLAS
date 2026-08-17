#include "atlas/landmark.hpp"

#include <algorithm>
#include <functional>
#include <limits>
#include <queue>
#include <stdexcept>
#include <utility>

namespace atlas {
namespace {

constexpr double infinity = std::numeric_limits<double>::infinity();
using Adjacency = std::vector<std::vector<Edge>>;

std::vector<double> all_distances(const Adjacency& adjacency, NodeId source) {
    std::vector<double> distances(adjacency.size(), infinity);
    using QueueEntry = std::pair<double, NodeId>;
    std::priority_queue<QueueEntry, std::vector<QueueEntry>, std::greater<>> frontier;
    distances[source] = 0.0;
    frontier.push({0.0, source});

    while (!frontier.empty()) {
        const auto [current_distance, current] = frontier.top();
        frontier.pop();
        if (current_distance != distances[current]) {
            continue;
        }
        for (const Edge& edge : adjacency[current]) {
            const double candidate = current_distance + edge.weight;
            if (candidate < distances[edge.to]) {
                distances[edge.to] = candidate;
                frontier.push({candidate, edge.to});
            }
        }
    }
    return distances;
}

}  // namespace

LandmarkIndex LandmarkIndex::build(const Graph& graph, const std::vector<NodeId>& landmarks) {
    if (landmarks.empty()) {
        throw GraphError("at least one landmark is required");
    }
    std::vector<NodeId> sorted_landmarks = landmarks;
    std::sort(sorted_landmarks.begin(), sorted_landmarks.end());
    if (std::adjacent_find(sorted_landmarks.begin(), sorted_landmarks.end()) !=
        sorted_landmarks.end()) {
        throw GraphError("landmarks must be unique");
    }
    for (const NodeId landmark : sorted_landmarks) {
        if (!graph.contains(landmark)) {
            throw GraphError("landmark node does not exist");
        }
    }

    Adjacency forward(graph.node_count());
    Adjacency reverse(graph.node_count());
    for (NodeId from = 0; from < graph.node_count(); ++from) {
        for (const Edge& edge : graph.neighbors(from)) {
            forward[from].push_back(edge);
            reverse[edge.to].push_back({from, edge.weight});
        }
    }

    std::vector<Distances> distances;
    distances.reserve(sorted_landmarks.size());
    for (const NodeId landmark : sorted_landmarks) {
        distances.push_back({all_distances(forward, landmark),
                             all_distances(reverse, landmark)});
    }
    return LandmarkIndex(graph.revision(), std::move(sorted_landmarks), std::move(distances));
}

LandmarkIndex::LandmarkIndex(std::uint64_t graph_revision, std::vector<NodeId> landmarks,
                             std::vector<Distances> distances)
    : graph_revision_(graph_revision),
      landmarks_(std::move(landmarks)),
      distances_(std::move(distances)) {}

double LandmarkIndex::heuristic(NodeId from, NodeId goal) const {
    if (from >= distances_.front().from_landmark.size() ||
        goal >= distances_.front().from_landmark.size()) {
        throw GraphError("heuristic endpoint does not exist");
    }
    double lower_bound = 0.0;
    for (const Distances& distance : distances_) {
        const double from_landmark_bound = distance.from_landmark[goal] -
                                            distance.from_landmark[from];
        const double to_landmark_bound = distance.to_landmark[from] - distance.to_landmark[goal];
        lower_bound = std::max({lower_bound, from_landmark_bound, to_landmark_bound});
    }
    return lower_bound;
}

bool LandmarkIndex::matches_revision(std::uint64_t graph_revision) const noexcept {
    return graph_revision_ == graph_revision;
}

std::size_t LandmarkIndex::landmark_count() const noexcept {
    return landmarks_.size();
}

}  // namespace atlas
