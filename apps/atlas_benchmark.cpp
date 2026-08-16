#include <iostream>

#include "atlas/benchmark.hpp"

int main() {
    const auto graph = atlas::make_benchmark_graph();
    const auto queries = atlas::make_benchmark_queries();
    for (const auto& row : atlas::benchmark_algorithms(graph, queries)) {
        std::cout << row.algorithm << " bucket=" << row.bucket << " queries=" << row.query_count
                  << " successful=" << row.successful_queries
                  << " correctness=" << (row.correctness_passed ? "pass" : "fail")
                  << " nodes=" << row.nodes_expanded
                  << " queue_pushes=" << row.queue_pushes
                  << " total_ms=" << row.total_milliseconds << '\n';
    }
    return 0;
}
