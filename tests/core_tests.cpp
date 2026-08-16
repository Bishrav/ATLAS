#include <iostream>

#include "atlas/core.hpp"
#include "atlas/graph.hpp"

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
    return 0;
}
