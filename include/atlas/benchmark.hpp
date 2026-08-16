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
    double total_milliseconds;
};

struct BenchmarkQuery {
    NodeId start;
    NodeId goal;
    std::string bucket;
};

[[nodiscard]] std::vector<BenchmarkRow> benchmark_algorithms(
    const Graph& graph, const std::vector<BenchmarkQuery>& queries);

[[nodiscard]] Graph make_benchmark_graph();
[[nodiscard]] std::vector<BenchmarkQuery> make_benchmark_queries();

}  // namespace atlas
