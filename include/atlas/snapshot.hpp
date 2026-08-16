#pragma once

#include <istream>
#include <stdexcept>
#include <string>

#include "atlas/graph.hpp"

namespace atlas {

class SnapshotError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

[[nodiscard]] std::string serialize_graph(const Graph& graph);
[[nodiscard]] Graph deserialize_graph(std::istream& input);
[[nodiscard]] Graph load_graph_snapshot(const std::string& path);
void save_graph_snapshot(const Graph& graph, const std::string& path);

}  // namespace atlas
