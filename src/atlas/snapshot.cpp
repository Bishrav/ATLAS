#include "atlas/snapshot.hpp"

#include <fstream>
#include <iomanip>
#include <sstream>

namespace atlas {
namespace {

constexpr const char* snapshot_header = "ATLAS_GRAPH_SNAPSHOT_V1";

void require_token(std::istream& input, const char* expected) {
    std::string actual;
    if (!(input >> actual) || actual != expected) {
        throw SnapshotError(std::string("expected snapshot token: ") + expected);
    }
}

}  // namespace

std::string serialize_graph(const Graph& graph) {
    std::ostringstream output;
    output << snapshot_header << '\n';
    output << "nodes " << graph.node_count() << '\n';
    output << std::setprecision(17);
    for (NodeId node = 0; node < graph.node_count(); ++node) {
        const auto point = graph.coordinate(node);
        output << "node " << node;
        if (point) {
            output << ' ' << point->x << ' ' << point->y;
        } else {
            output << " none";
        }
        output << '\n';
    }
    output << "edges " << graph.edge_count() << '\n';
    for (NodeId from = 0; from < graph.node_count(); ++from) {
        for (const Edge& edge : graph.neighbors(from)) {
            output << "edge " << from << ' ' << edge.to << ' ' << edge.weight << '\n';
        }
    }
    return output.str();
}

Graph deserialize_graph(std::istream& input) {
    std::string header;
    if (!(input >> header) || header != snapshot_header) {
        throw SnapshotError("unsupported or missing graph snapshot header");
    }
    require_token(input, "nodes");
    std::size_t node_count = 0;
    if (!(input >> node_count)) {
        throw SnapshotError("invalid node count");
    }
    Graph graph(node_count);
    for (NodeId expected = 0; expected < node_count; ++expected) {
        require_token(input, "node");
        NodeId node = 0;
        std::string x_token;
        if (!(input >> node >> x_token) || node != expected) {
            throw SnapshotError("node records must be contiguous and ordered");
        }
        if (x_token != "none") {
            double x = 0.0;
            double y = 0.0;
            try {
                x = std::stod(x_token);
            } catch (const std::exception&) {
                throw SnapshotError("invalid node coordinate");
            }
            if (!(input >> y)) {
                throw SnapshotError("node coordinate requires x and y");
            }
            try {
                graph.set_coordinate(node, {x, y});
            } catch (const GraphError& error) {
                throw SnapshotError(error.what());
            }
        }
    }
    require_token(input, "edges");
    std::size_t edge_count = 0;
    if (!(input >> edge_count)) {
        throw SnapshotError("invalid edge count");
    }
    for (std::size_t index = 0; index < edge_count; ++index) {
        require_token(input, "edge");
        NodeId from = 0;
        NodeId to = 0;
        double weight = 0.0;
        if (!(input >> from >> to >> weight)) {
            throw SnapshotError("invalid edge record");
        }
        try {
            graph.add_edge(from, to, weight);
        } catch (const GraphError& error) {
            throw SnapshotError(error.what());
        }
    }
    return graph;
}

Graph load_graph_snapshot(const std::string& path) {
    std::ifstream input(path);
    if (!input) {
        throw SnapshotError("unable to open graph snapshot: " + path);
    }
    return deserialize_graph(input);
}

void save_graph_snapshot(const Graph& graph, const std::string& path) {
    std::ofstream output(path);
    if (!output) {
        throw SnapshotError("unable to write graph snapshot: " + path);
    }
    output << serialize_graph(graph);
    if (!output) {
        throw SnapshotError("unable to finish graph snapshot: " + path);
    }
}

}  // namespace atlas
