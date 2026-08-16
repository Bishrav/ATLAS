#include <cmath>
#include <iostream>
#include <limits>
#include <random>
#include <sstream>
#include <vector>

#include "atlas/core.hpp"
#include "atlas/graph.hpp"
#include "atlas/graph_io.hpp"
#include "atlas/traversal.hpp"
#include "atlas/shortest_path.hpp"
#include "atlas/a_star.hpp"
#include "atlas/bidirectional.hpp"
#include "atlas/benchmark.hpp"
#include "atlas/snapshot.hpp"

namespace {

double oracle_distance(const atlas::Graph& graph, atlas::NodeId start, atlas::NodeId goal) {
    const double infinity = std::numeric_limits<double>::infinity();
    std::vector<double> distance(graph.node_count(), infinity);
    distance[start] = 0.0;
    for (std::size_t pass = 1; pass < graph.node_count(); ++pass) {
        bool changed = false;
        for (atlas::NodeId from = 0; from < graph.node_count(); ++from) {
            if (distance[from] == infinity) {
                continue;
            }
            for (const atlas::Edge& edge : graph.neighbors(from)) {
                const double candidate = distance[from] + edge.weight;
                if (candidate < distance[edge.to]) {
                    distance[edge.to] = candidate;
                    changed = true;
                }
            }
        }
        if (!changed) {
            break;
        }
    }
    return distance[goal];
}

}  // namespace

int main() {
    if (atlas::version() != "0.1.0") {
        std::cerr << "Unexpected ATLAS version\n";
        return 1;
    }
    atlas::Graph graph(3);
    graph.add_edge(0, 1, 12.5);
    graph.add_edge(0, 2, 25.0);
    if (graph.node_count() != 3 || graph.edge_count() != 2 || graph.neighbors(0).size() != 2) {
        std::cerr << "Graph contract counts are incorrect\n";
        return 1;
    }
    try {
        graph.add_edge(0, 1, 9.0);
        return 1;
    } catch (const atlas::GraphError&) {
    }
    try {
        graph.add_edge(1, 2, -1.0);
        return 1;
    } catch (const atlas::GraphError&) {
    }
    std::istringstream input("# fixture\nnode 0\nnode 1\nedge 0 1 4.5\n");
    const atlas::Graph parsed = atlas::parse_graph(input);
    if (parsed.node_count() != 2 || parsed.edge_count() != 1 || parsed.neighbors(0)[0].weight != 4.5) {
        std::cerr << "Graph parser output is incorrect\n";
        return 1;
    }
    try {
        std::istringstream invalid("node 0\nedge 0 9 1\n");
        static_cast<void>(atlas::parse_graph(invalid));
        return 1;
    } catch (const atlas::GraphParseError& error) {
        if (std::string(error.what()).find("line 2") == std::string::npos) {
            return 1;
        }
    }
    const auto breadth_first = atlas::breadth_first_order(parsed, 0);
    const auto depth_first = atlas::depth_first_order(parsed, 0);
    if (breadth_first != std::vector<atlas::NodeId>{0, 1} ||
        depth_first != std::vector<atlas::NodeId>{0, 1}) {
        std::cerr << "Traversal order is incorrect\n";
        return 1;
    }
    atlas::Graph routes(4);
    routes.add_edge(0, 1, 5.0);
    routes.add_edge(0, 2, 1.0);
    routes.add_edge(2, 1, 1.0);
    routes.add_edge(1, 3, 3.0);
    routes.add_edge(2, 3, 10.0);
    const auto shortest = atlas::dijkstra(routes, 0, 3);
    if (!shortest.reachable || shortest.cost != 5.0 ||
        shortest.nodes != std::vector<atlas::NodeId>{0, 2, 1, 3}) {
        std::cerr << "Dijkstra result is incorrect\n";
        return 1;
    }
    const auto unreachable = atlas::dijkstra(routes, 3, 0);
    if (unreachable.reachable || !unreachable.nodes.empty()) {
        std::cerr << "Unreachable Dijkstra result is incorrect\n";
        return 1;
    }
    constexpr atlas::NodeId random_node_count = 12;
    atlas::Graph random_graph(random_node_count);
    std::mt19937 generator(20260816);
    std::uniform_int_distribution<int> include_edge(0, 3);
    std::uniform_int_distribution<int> weight(1, 20);
    for (atlas::NodeId from = 0; from < random_node_count; ++from) {
        if (from + 1 < random_node_count) {
            random_graph.add_edge(from, from + 1, static_cast<double>(weight(generator)));
        }
        for (atlas::NodeId to = 0; to < random_node_count; ++to) {
            if (from != to && to != from + 1 && include_edge(generator) == 0) {
                try {
                    random_graph.add_edge(from, to, static_cast<double>(weight(generator)));
                } catch (const atlas::GraphError&) {
                }
            }
        }
    }
    for (atlas::NodeId start = 0; start < random_node_count; ++start) {
        for (atlas::NodeId goal = 0; goal < random_node_count; ++goal) {
            const auto actual = atlas::dijkstra(random_graph, start, goal);
            const double expected = oracle_distance(random_graph, start, goal);
    if (std::isinf(expected) != !actual.reachable ||
                (actual.reachable && std::abs(actual.cost - expected) > 1e-9)) {
                std::cerr << "Randomized Dijkstra oracle mismatch\n";
                return 1;
            }
        }
    }
    const auto benchmark_graph = atlas::make_benchmark_graph();
    const auto benchmark_queries = atlas::make_benchmark_queries();
    const auto benchmark_rows = atlas::benchmark_algorithms(benchmark_graph, benchmark_queries);
    if (benchmark_rows.size() != 9) {
        std::cerr << "Benchmark bucket algorithm count is incorrect\n";
        return 1;
    }
    for (const auto& row : benchmark_rows) {
        if (row.query_count == 0 || row.successful_queries != row.query_count ||
            !row.correctness_passed || row.nodes_expanded == 0 || row.queue_pushes == 0 ||
            row.total_milliseconds < 0.0) {
            std::cerr << "Benchmark result is incorrect\n";
            return 1;
        }
    }
    const std::string snapshot = atlas::serialize_graph(benchmark_graph);
    std::istringstream snapshot_input(snapshot);
    const atlas::Graph restored = atlas::deserialize_graph(snapshot_input);
    if (atlas::serialize_graph(restored) != snapshot ||
        restored.node_count() != benchmark_graph.node_count() ||
        restored.edge_count() != benchmark_graph.edge_count()) {
        std::cerr << "Graph snapshot round trip is incorrect\n";
        return 1;
    }
    const auto regression = atlas::compare_benchmarks(benchmark_rows, benchmark_rows);
    if (regression.size() != benchmark_rows.size()) {
        std::cerr << "Benchmark regression result count is incorrect\n";
        return 1;
    }
    for (const auto& result : regression) {
        if (!result.passed) {
            std::cerr << "Identical benchmark reports should pass regression checks\n";
            return 1;
        }
    }
    const auto environment = atlas::benchmark_environment();
    if (environment.compiler.empty() || environment.language_standard.empty() ||
        environment.platform.empty()) {
        std::cerr << "Benchmark environment metadata is incomplete\n";
        return 1;
    }
    atlas::Graph geographic(4);
    geographic.set_coordinate(0, {0.0, 0.0});
    geographic.set_coordinate(1, {1.0, 0.0});
    geographic.set_coordinate(2, {2.0, 0.0});
    geographic.set_coordinate(3, {3.0, 0.0});
    geographic.add_edge(0, 1, 1.0);
    geographic.add_edge(1, 3, 2.0);
    geographic.add_edge(0, 2, 2.0);
    geographic.add_edge(2, 3, 1.0);
    const auto dijkstra_route = atlas::dijkstra(geographic, 0, 3);
    const auto a_star_route = atlas::a_star(geographic, 0, 3);
    if (!a_star_route.reachable || a_star_route.cost != dijkstra_route.cost ||
        a_star_route.nodes != dijkstra_route.nodes) {
        std::cerr << "A* result does not match Dijkstra\n";
        return 1;
    }
    const auto bidirectional_route = atlas::bidirectional_dijkstra(geographic, 0, 3);
    if (!bidirectional_route.reachable || bidirectional_route.cost != dijkstra_route.cost ||
        bidirectional_route.nodes.front() != 0 || bidirectional_route.nodes.back() != 3) {
        std::cerr << "Bidirectional Dijkstra result is incorrect\n";
        return 1;
    }
    try {
        atlas::Graph missing_coordinates(2);
        missing_coordinates.add_edge(0, 1, 1.0);
        static_cast<void>(atlas::a_star(missing_coordinates, 0, 1));
        return 1;
    } catch (const atlas::GraphError&) {
    }
    const auto no_route = atlas::bidirectional_dijkstra(geographic, 3, 0);
    if (no_route.reachable || !no_route.nodes.empty()) {
        std::cerr << "Bidirectional unreachable result is incorrect\n";
        return 1;
    }
    return 0;
}
