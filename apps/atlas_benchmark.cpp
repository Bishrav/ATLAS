#include <iostream>

#include "atlas/benchmark.hpp"
#include "atlas/landmark.hpp"

int main() {
    const auto environment = atlas::benchmark_environment();
    std::cout << "environment compiler=" << environment.compiler
              << " standard=" << environment.language_standard
              << " platform=" << environment.platform << '\n';
    const auto graph = atlas::make_benchmark_graph();
    const auto queries = atlas::make_benchmark_queries();
    const auto landmarks = atlas::LandmarkIndex::build(
        graph, {0, static_cast<atlas::NodeId>(graph.node_count() - 1)});
    std::cout << "alt_landmarks=" << landmarks.landmark_count()
              << " alt_index_memory_bytes=" << landmarks.estimated_memory_bytes() << '\n';
    for (const auto& row : atlas::benchmark_algorithms(graph, queries)) {
        std::cout << row.algorithm << " bucket=" << row.bucket << " queries=" << row.query_count
                  << " successful=" << row.successful_queries
                  << " correctness=" << (row.correctness_passed ? "pass" : "fail")
                  << " nodes=" << row.nodes_expanded
                  << " queue_pushes=" << row.queue_pushes
                  << " graph_memory_bytes=" << row.estimated_graph_memory_bytes
                  << " total_ms=" << row.total_milliseconds << '\n';
    }
    return 0;
}
