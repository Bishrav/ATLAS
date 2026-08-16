#pragma once

#include <cstddef>
#include <cstdint>
#include <list>
#include <optional>
#include <unordered_map>

#include "atlas/shortest_path.hpp"

namespace atlas {

struct RouteCacheStats {
    std::size_t hits = 0;
    std::size_t misses = 0;
    std::size_t evictions = 0;
};

class RouteCache {
public:
    explicit RouteCache(std::size_t capacity);

    [[nodiscard]] std::optional<PathResult> get(
        NodeId start, NodeId goal, std::uint64_t graph_revision);
    void put(NodeId start, NodeId goal, std::uint64_t graph_revision, PathResult result);
    void clear();

    [[nodiscard]] std::size_t size() const noexcept;
    [[nodiscard]] RouteCacheStats stats() const noexcept;

private:
    struct Key {
        NodeId start;
        NodeId goal;
        std::uint64_t graph_revision;

        bool operator==(const Key&) const = default;
    };

    struct KeyHash {
        std::size_t operator()(const Key& key) const noexcept;
    };

    struct Entry {
        Key key;
        PathResult result;
    };

    using Entries = std::list<Entry>;
    using Index = std::unordered_map<Key, Entries::iterator, KeyHash>;

    std::size_t capacity_;
    Entries entries_;
    Index index_;
    RouteCacheStats stats_;
};

}  // namespace atlas
