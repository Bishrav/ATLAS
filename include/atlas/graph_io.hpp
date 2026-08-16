#pragma once

#include <istream>
#include <stdexcept>
#include <string>

#include "atlas/graph.hpp"

namespace atlas {

class GraphParseError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

[[nodiscard]] Graph parse_graph(std::istream& input);
[[nodiscard]] Graph load_graph_file(const std::string& path);

}  // namespace atlas
