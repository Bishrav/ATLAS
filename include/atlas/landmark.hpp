#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

#include "atlas/graph.hpp"

namespace atlas {

class LandmarkIndex {
public:
    static LandmarkIndex build(const Graph& graph, const std::vector<NodeId>& landmarks);

    [[nodiscard]] double heuristic(NodeId from, NodeId goal) const;
    [[nodiscard]] bool matches_revision(std::uint64_t graph_revision) const noexcept;
    [[nodiscard]] std::size_t landmark_count() const noexcept;
    [[nodiscard]] std::size_t estimated_memory_bytes() const noexcept;

private:
    struct Distances {
        std::vector<double> from_landmark;
        std::vector<double> to_landmark;
    };

    LandmarkIndex(std::uint64_t graph_revision, std::vector<NodeId> landmarks,
                  std::vector<Distances> distances);

    std::uint64_t graph_revision_;
    std::vector<NodeId> landmarks_;
    std::vector<Distances> distances_;
};

}  // namespace atlas
