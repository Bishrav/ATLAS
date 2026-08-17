#include <iostream>
#include <string>
#include <string_view>

#include "atlas/a_star.hpp"
#include "atlas/benchmark.hpp"
#include "atlas/bidirectional.hpp"
#include "atlas/core.hpp"
#include "atlas/shortest_path.hpp"

namespace {

void print_route(const atlas::PathResult& result) {
    std::cout << "reachable=" << (result.reachable ? "true" : "false")
              << " cost=" << result.cost << " nodes_expanded=" << result.metrics.nodes_expanded
              << " queue_pushes=" << result.metrics.queue_pushes << " route=";
    for (std::size_t index = 0; index < result.nodes.size(); ++index) {
        if (index != 0) {
            std::cout << ',';
        }
        std::cout << result.nodes[index];
    }
    std::cout << '\n';
}

}  // namespace

int main(int argc, char* argv[]) {
    if (argc == 2 && std::string_view(argv[1]) == "--version") {
        std::cout << atlas::version() << '\n';
        return 0;
    }
    if (argc == 2 || (argc == 4 && std::string_view(argv[1]) == "demo-route" &&
                      std::string_view(argv[2]) == "--algorithm")) {
        if (std::string_view(argv[1]) != "demo-route") {
            return 2;
        }
        const std::string_view algorithm = argc == 2 ? "dijkstra" : argv[3];
        const auto graph = atlas::make_benchmark_graph();
        if (algorithm == "dijkstra") {
            print_route(atlas::dijkstra(graph, 0, 31));
        } else if (algorithm == "a-star") {
            print_route(atlas::a_star(graph, 0, 31));
        } else if (algorithm == "bidirectional") {
            print_route(atlas::bidirectional_dijkstra(graph, 0, 31));
        } else {
            std::cerr << "Unknown algorithm: " << algorithm << '\n';
            return 2;
        }
        return 0;
    }

    std::cout << "ATLAS routing and optimization engine\n"
              << "Usage: atlas --version\n"
              << "       atlas demo-route [--algorithm dijkstra|a-star|bidirectional]\n";
    return argc == 1 ? 0 : 2;
}
