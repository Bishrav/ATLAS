#pragma once

#include <cstddef>
#include <string>
#include <utility>
#include <vector>

#include "atlas/graph.hpp"
#include "atlas/shortest_path.hpp"

namespace atlas {

struct BenchmarkRow {
    std::string algorithm;
    std::string bucket;
    std::size_t query_count;
    std::size_t successful_queries;
    bool correctness_passed;
    std::size_t nodes_expanded;
    std::size_t queue_pushes;
    std::size_t estimated_graph_memory_bytes;
    double total_milliseconds;
};

struct BenchmarkQuery {
    NodeId start;
    NodeId goal;
    std::string bucket;
};

struct BenchmarkRegression {
    std::string algorithm;
    std::string bucket;
    bool passed;
    std::string reason;
};

struct BenchmarkEnvironment {
    std::string compiler;
    std::string language_standard;
    std::string platform;
};

[[nodiscard]] std::vector<BenchmarkRow> benchmark_algorithms(
    const Graph& graph, const std::vector<BenchmarkQuery>& queries);

[[nodiscard]] std::vector<BenchmarkRegression> compare_benchmarks(
    const std::vector<BenchmarkRow>& current, const std::vector<BenchmarkRow>& baseline,
    double maximum_slowdown = 1.25);

[[nodiscard]] BenchmarkEnvironment benchmark_environment();

[[nodiscard]] Graph make_benchmark_graph();
[[nodiscard]] std::vector<BenchmarkQuery> make_benchmark_queries();

}  // namespace atlas
