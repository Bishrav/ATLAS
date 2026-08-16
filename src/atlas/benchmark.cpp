#include "atlas/benchmark.hpp"

#include <chrono>

#include "atlas/a_star.hpp"
#include "atlas/bidirectional.hpp"
#include "atlas/shortest_path.hpp"

namespace atlas {
namespace {

template <typename Search>
BenchmarkRow measure(const std::string& name, const Graph& graph,
                     const std::vector<std::pair<NodeId, NodeId>>& queries, Search search) {
    const auto started = std::chrono::steady_clock::now();
    std::size_t successful = 0;
    for (const auto [start, goal] : queries) {
        if (search(graph, start, goal).reachable) {
            ++successful;
        }
    }
    const auto finished = std::chrono::steady_clock::now();
    const double elapsed = std::chrono::duration<double, std::milli>(finished - started).count();
    return {name, queries.size(), successful, elapsed};
}

}  // namespace

std::vector<BenchmarkRow> benchmark_algorithms(
    const Graph& graph, const std::vector<std::pair<NodeId, NodeId>>& queries) {
    return {
        measure("dijkstra", graph, queries, dijkstra),
        measure("a_star", graph, queries, a_star),
        measure("bidirectional_dijkstra", graph, queries, bidirectional_dijkstra),
    };
}

Graph make_benchmark_graph() {
    constexpr NodeId node_count = 32;
    Graph graph(node_count);
    for (NodeId node = 0; node < node_count; ++node) {
        graph.set_coordinate(node, {static_cast<double>(node), 0.0});
        if (node + 1 < node_count) {
            graph.add_edge(node, node + 1, 1.0);
        }
        if (node + 4 < node_count) {
            graph.add_edge(node, node + 4, 4.0);
        }
    }
    return graph;
}

std::vector<std::pair<NodeId, NodeId>> make_benchmark_queries() {
    return {{0, 31}, {3, 27}, {8, 16}, {12, 30}, {4, 30}, {7, 7}};
}

}  // namespace atlas
