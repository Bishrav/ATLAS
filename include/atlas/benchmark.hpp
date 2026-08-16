#pragma once

#include <cstddef>
#include <string>
#include <utility>
#include <vector>

#include "atlas/graph.hpp"

namespace atlas {

struct BenchmarkRow {
    std::string algorithm;
    std::size_t query_count;
    std::size_t successful_queries;
    double total_milliseconds;
};

[[nodiscard]] std::vector<BenchmarkRow> benchmark_algorithms(
    const Graph& graph, const std::vector<std::pair<NodeId, NodeId>>& queries);

[[nodiscard]] Graph make_benchmark_graph();
[[nodiscard]] std::vector<std::pair<NodeId, NodeId>> make_benchmark_queries();

}  // namespace atlas
