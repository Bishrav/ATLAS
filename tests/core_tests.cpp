#include <iostream>
#include <sstream>

#include "atlas/core.hpp"
#include "atlas/graph.hpp"
#include "atlas/graph_io.hpp"
#include "atlas/traversal.hpp"
#include "atlas/shortest_path.hpp"

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
    return 0;
}
