#include "atlas/route_cache.hpp"

#include <iterator>
#include <stdexcept>
#include <utility>

namespace atlas {

RouteCache::RouteCache(std::size_t capacity) : capacity_(capacity) {
    if (capacity == 0) {
        throw std::invalid_argument("route cache capacity must be positive");
    }
}

std::size_t RouteCache::KeyHash::operator()(const Key& key) const noexcept {
    std::size_t value = static_cast<std::size_t>(key.start);
    value ^= static_cast<std::size_t>(key.goal) + 0x9e3779b9U + (value << 6U) + (value >> 2U);
    value ^= static_cast<std::size_t>(key.graph_revision) + 0x9e3779b9U + (value << 6U) + (value >> 2U);
    return value;
}

std::optional<PathResult> RouteCache::get(
    NodeId start, NodeId goal, std::uint64_t graph_revision) {
    const Key key{start, goal, graph_revision};
    const auto found = index_.find(key);
    if (found == index_.end()) {
        ++stats_.misses;
        return std::nullopt;
    }
    entries_.splice(entries_.begin(), entries_, found->second);
    ++stats_.hits;
    return entries_.front().result;
}

void RouteCache::put(NodeId start, NodeId goal, std::uint64_t graph_revision, PathResult result) {
    const Key key{start, goal, graph_revision};
    const auto found = index_.find(key);
    if (found != index_.end()) {
        found->second->result = std::move(result);
        entries_.splice(entries_.begin(), entries_, found->second);
        return;
    }
    entries_.push_front({key, std::move(result)});
    index_[key] = entries_.begin();
    if (entries_.size() > capacity_) {
        const auto evicted = std::prev(entries_.end());
        index_.erase(evicted->key);
        entries_.pop_back();
        ++stats_.evictions;
    }
}

std::size_t RouteCache::invalidate_older_than(std::uint64_t graph_revision) {
    std::size_t removed = 0;
    for (auto entry = entries_.begin(); entry != entries_.end();) {
        if (entry->key.graph_revision < graph_revision) {
            index_.erase(entry->key);
            entry = entries_.erase(entry);
            ++removed;
        } else {
            ++entry;
        }
    }
    return removed;
}

void RouteCache::clear() {
    entries_.clear();
    index_.clear();
}

std::size_t RouteCache::size() const noexcept {
    return entries_.size();
}

RouteCacheStats RouteCache::stats() const noexcept {
    return stats_;
}

}  // namespace atlas
