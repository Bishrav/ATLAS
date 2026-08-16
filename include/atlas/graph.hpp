#pragma once

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace atlas {

using NodeId = std::uint32_t;

struct Edge {
    NodeId to;
    double weight;
};

class GraphError : public std::invalid_argument {
public:
    using std::invalid_argument::invalid_argument;
};

class Graph {
public:
    explicit Graph(std::size_t node_count = 0);

    void add_node(NodeId id);
    void add_edge(NodeId from, NodeId to, double weight);

    [[nodiscard]] bool contains(NodeId id) const noexcept;
    [[nodiscard]] std::size_t node_count() const noexcept;
    [[nodiscard]] std::size_t edge_count() const noexcept;
    [[nodiscard]] const std::vector<Edge>& neighbors(NodeId from) const;

private:
    std::vector<std::vector<Edge>> adjacency_;
    std::size_t edge_count_ = 0;
};

}  // namespace atlas
