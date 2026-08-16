#include "atlas/graph_io.hpp"

#include <fstream>
#include <limits>
#include <sstream>
#include <string_view>

namespace atlas {
namespace {

std::string line_error(std::size_t line_number, std::string_view message) {
    return "line " + std::to_string(line_number) + ": " + std::string(message);
}

NodeId parse_node_id(const std::string& token, std::size_t line_number) {
    try {
        std::size_t consumed = 0;
        const unsigned long long value = std::stoull(token, &consumed);
        if (consumed != token.size() || value > std::numeric_limits<NodeId>::max()) {
            throw std::invalid_argument("invalid node ID");
        }
        return static_cast<NodeId>(value);
    } catch (const std::exception&) {
        throw GraphParseError(line_error(line_number, "node ID must be an unsigned 32-bit integer"));
    }
}

double parse_weight(const std::string& token, std::size_t line_number) {
    try {
        std::size_t consumed = 0;
        const double value = std::stod(token, &consumed);
        if (consumed != token.size()) {
            throw std::invalid_argument("invalid weight");
        }
        return value;
    } catch (const std::exception&) {
        throw GraphParseError(line_error(line_number, "weight must be a number"));
    }
}

}  // namespace

Graph parse_graph(std::istream& input) {
    Graph graph;
    std::string line;
    std::size_t line_number = 0;
    while (std::getline(input, line)) {
        ++line_number;
        std::istringstream tokens(line);
        std::string record;
        if (!(tokens >> record) || record[0] == '#') {
            continue;
        }

        std::string first;
        std::string second;
        std::string third;
        if (record == "node") {
            if (!(tokens >> first) || (tokens >> second)) {
                throw GraphParseError(line_error(line_number, "node record requires exactly one ID"));
            }
            try {
                graph.add_node(parse_node_id(first, line_number));
            } catch (const GraphError& error) {
                throw GraphParseError(line_error(line_number, error.what()));
            }
        } else if (record == "edge") {
            if (!(tokens >> first >> second >> third) || (tokens >> line)) {
                throw GraphParseError(line_error(line_number, "edge record requires from, to, and weight"));
            }
            try {
                graph.add_edge(
                    parse_node_id(first, line_number),
                    parse_node_id(second, line_number),
                    parse_weight(third, line_number));
            } catch (const GraphError& error) {
                throw GraphParseError(line_error(line_number, error.what()));
            }
        } else {
            throw GraphParseError(line_error(line_number, "record must be node or edge"));
        }
    }
    if (input.bad()) {
        throw GraphParseError("unable to read graph input");
    }
    return graph;
}

Graph load_graph_file(const std::string& path) {
    std::ifstream input(path);
    if (!input) {
        throw GraphParseError("unable to open graph file: " + path);
    }
    return parse_graph(input);
}

}  // namespace atlas
