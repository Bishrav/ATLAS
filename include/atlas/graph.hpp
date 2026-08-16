#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <stdexcept>
#include <vector>

namespace atlas {

using NodeId = std::uint32_t;

struct Edge {
    NodeId to;
    double weight;
};

struct Coordinate {
    double x;
    double y;
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
    void update_edge_weight(NodeId from, NodeId to, double weight);
    void remove_edge(NodeId from, NodeId to);
    void set_coordinate(NodeId id, Coordinate coordinate);

    [[nodiscard]] bool contains(NodeId id) const noexcept;
    [[nodiscard]] std::size_t node_count() const noexcept;
    [[nodiscard]] std::size_t edge_count() const noexcept;
    [[nodiscard]] std::size_t estimated_memory_bytes() const noexcept;
    [[nodiscard]] std::uint64_t revision() const noexcept;
    [[nodiscard]] const std::vector<Edge>& neighbors(NodeId from) const;
    [[nodiscard]] std::optional<Coordinate> coordinate(NodeId id) const;

private:
    std::vector<std::vector<Edge>> adjacency_;
    std::vector<std::optional<Coordinate>> coordinates_;
    std::size_t edge_count_ = 0;
    std::uint64_t revision_ = 0;
};

}  // namespace atlas
