#include "atlas/benchmark.hpp"

#include <chrono>
#include <cmath>

#include "atlas/a_star.hpp"
#include "atlas/bidirectional.hpp"
#include "atlas/shortest_path.hpp"

namespace atlas {
namespace {

template <typename Search>
BenchmarkRow measure(const std::string& name, const Graph& graph,
                     const std::string& bucket, const std::vector<BenchmarkQuery>& queries, Search search,
                     const std::vector<PathResult>& expected) {
    const auto started = std::chrono::steady_clock::now();
    std::size_t successful = 0;
    bool correctness_passed = true;
    std::size_t nodes_expanded = 0;
    std::size_t queue_pushes = 0;
    for (std::size_t index = 0; index < queries.size(); ++index) {
        const BenchmarkQuery& query = queries[index];
        const NodeId start = query.start;
        const NodeId goal = query.goal;
        const PathResult actual = search(graph, start, goal);
        if (actual.reachable) {
            ++successful;
        }
        if (actual.reachable != expected[index].reachable ||
            (actual.reachable && std::abs(actual.cost - expected[index].cost) > 1e-9)) {
            correctness_passed = false;
        }
        nodes_expanded += actual.metrics.nodes_expanded;
        queue_pushes += actual.metrics.queue_pushes;
    }
    const auto finished = std::chrono::steady_clock::now();
    const double elapsed = std::chrono::duration<double, std::milli>(finished - started).count();
    return {name, bucket, queries.size(), successful, correctness_passed,
            nodes_expanded, queue_pushes, elapsed};
}

}  // namespace

std::vector<BenchmarkRow> benchmark_algorithms(
    const Graph& graph, const std::vector<BenchmarkQuery>& queries) {
    std::vector<BenchmarkRow> rows;
    for (const std::string& bucket : {std::string("short"), std::string("medium"), std::string("long")}) {
        std::vector<BenchmarkQuery> bucket_queries;
        for (const BenchmarkQuery& query : queries) {
            if (query.bucket == bucket) {
                bucket_queries.push_back(query);
            }
        }
        if (bucket_queries.empty()) {
            continue;
        }
        std::vector<PathResult> expected;
        expected.reserve(bucket_queries.size());
        for (const BenchmarkQuery& query : bucket_queries) {
            expected.push_back(dijkstra(graph, query.start, query.goal));
        }
        rows.push_back(measure("dijkstra", graph, bucket, bucket_queries, dijkstra, expected));
        rows.push_back(measure("a_star", graph, bucket, bucket_queries, a_star, expected));
        rows.push_back(measure("bidirectional_dijkstra", graph, bucket, bucket_queries,
                               bidirectional_dijkstra, expected));
    }
    return rows;
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

std::vector<BenchmarkQuery> make_benchmark_queries() {
    return {{0, 4, "short"}, {8, 16, "short"}, {3, 15, "medium"},
            {12, 30, "medium"}, {0, 31, "long"}, {4, 30, "long"}};
}

}  // namespace atlas
