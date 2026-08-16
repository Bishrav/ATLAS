#include <iostream>
#include <sstream>

#include "atlas/core.hpp"
#include "atlas/graph.hpp"
#include "atlas/graph_io.hpp"

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
    return 0;
}
