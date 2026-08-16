#include "atlas/benchmark.hpp"

#include <chrono>
#include <cmath>
#include <map>
#include <stdexcept>

#if defined(_WIN32)
#define ATLAS_PLATFORM "windows"
#elif defined(__linux__)
#define ATLAS_PLATFORM "linux"
#elif defined(__APPLE__)
#define ATLAS_PLATFORM "macos"
#else
#define ATLAS_PLATFORM "unknown"
#endif

#if defined(_MSC_VER)
#define ATLAS_COMPILER "msvc"
#elif defined(__clang__)
#define ATLAS_COMPILER "clang"
#elif defined(__GNUC__)
#define ATLAS_COMPILER "gcc"
#else
#define ATLAS_COMPILER "unknown"
#endif

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

std::vector<BenchmarkRegression> compare_benchmarks(
    const std::vector<BenchmarkRow>& current, const std::vector<BenchmarkRow>& baseline,
    double maximum_slowdown) {
    if (!(maximum_slowdown >= 1.0) || !std::isfinite(maximum_slowdown)) {
        throw std::invalid_argument("maximum slowdown must be finite and at least 1.0");
    }
    using Key = std::pair<std::string, std::string>;
    std::map<Key, BenchmarkRow> baseline_by_key;
    for (const BenchmarkRow& row : baseline) {
        baseline_by_key[{row.algorithm, row.bucket}] = row;
    }
    std::vector<BenchmarkRegression> results;
    for (const BenchmarkRow& row : current) {
        const Key key{row.algorithm, row.bucket};
        const auto baseline_row = baseline_by_key.find(key);
        if (baseline_row == baseline_by_key.end()) {
            results.push_back({row.algorithm, row.bucket, false, "missing baseline row"});
            continue;
        }
        const BenchmarkRow& expected = baseline_row->second;
        if (!row.correctness_passed) {
            results.push_back({row.algorithm, row.bucket, false, "current correctness failed"});
        } else if (row.query_count != expected.query_count) {
            results.push_back({row.algorithm, row.bucket, false, "query count changed"});
        } else if (expected.total_milliseconds > 0.0 &&
                   row.total_milliseconds > expected.total_milliseconds * maximum_slowdown) {
            results.push_back({row.algorithm, row.bucket, false, "runtime regression"});
        } else {
            results.push_back({row.algorithm, row.bucket, true, "within configured threshold"});
        }
    }
    return results;
}

BenchmarkEnvironment benchmark_environment() {
    return {ATLAS_COMPILER, std::to_string(__cplusplus), ATLAS_PLATFORM};
}

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
