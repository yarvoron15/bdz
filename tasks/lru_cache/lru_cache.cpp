#include "lru_cache.h"

#include <list>
#include <unordered_map>

namespace {

struct CacheState {
    using List = std::list<std::pair<std::string, int>>;
    using Iterator = List::iterator;

    struct Entry {
        bool pinned = false;
        Iterator it;
    };

    List pinned;
    List unpinned;
    std::unordered_map<std::string, Entry> entries;
};

std::unordered_map<const LRUCache*, CacheState> g_states;

CacheState& State(const LRUCache* cache) {
    return g_states[cache];
}

void EvictIfNeeded(LRUCache* cache) {
    CacheState& state = State(cache);
    while (state.entries.size() > cache->capacity()) {
        if (state.unpinned.empty()) {
            break;
        }
        const std::string key = state.unpinned.front().first;
        state.unpinned.pop_front();
        state.entries.erase(key);
    }
}

}  // namespace

LRUCache::LRUCache(size_t capacity)
    : capacity_(capacity)
{
    g_states[this] = {};
}

size_t LRUCache::size() const
{
    return State(this).entries.size();
}

size_t LRUCache::capacity() const
{
    return capacity_;
}

void LRUCache::clear() noexcept
{
    CacheState& state = State(this);
    state.pinned.clear();
    state.unpinned.clear();
    state.entries.clear();
}

std::optional<int> LRUCache::get(const std::string& key)
{
    CacheState& state = State(this);
    auto map_it = state.entries.find(key);
    if (map_it == state.entries.end()) {
        return std::nullopt;
    }

    auto& entry = map_it->second;
    if (!entry.pinned) {
        state.unpinned.splice(state.unpinned.end(), state.unpinned, entry.it);
        entry.it = std::prev(state.unpinned.end());
    }
    return entry.it->second;
}

bool LRUCache::put(const std::string& key, int value)
{
    CacheState& state = State(this);
    auto map_it = state.entries.find(key);
    if (map_it != state.entries.end()) {
        map_it->second.it->second = value;
        if (!map_it->second.pinned) {
            state.unpinned.splice(state.unpinned.end(), state.unpinned, map_it->second.it);
            map_it->second.it = std::prev(state.unpinned.end());
        }
        return false;
    }

    if (capacity_ == 0) {
        return false;
    }
    if (state.entries.size() == capacity_ && state.unpinned.empty()) {
        return false;
    }

    state.unpinned.emplace_back(key, value);
    state.entries[key] = {false, std::prev(state.unpinned.end())};
    EvictIfNeeded(this);
    return true;
}

bool LRUCache::erase(const std::string& key)
{
    CacheState& state = State(this);
    auto map_it = state.entries.find(key);
    if (map_it == state.entries.end()) {
        return false;
    }

    if (map_it->second.pinned) {
        state.pinned.erase(map_it->second.it);
    } else {
        state.unpinned.erase(map_it->second.it);
    }
    state.entries.erase(map_it);
    return true;
}

bool LRUCache::pin(const std::string& key)
{
    CacheState& state = State(this);
    auto map_it = state.entries.find(key);
    if (map_it == state.entries.end() || map_it->second.pinned) {
        return false;
    }

    state.pinned.splice(state.pinned.end(), state.unpinned, map_it->second.it);
    map_it->second.pinned = true;
    map_it->second.it = std::prev(state.pinned.end());
    return true;
}

bool LRUCache::unpin(const std::string& key)
{
    CacheState& state = State(this);
    auto map_it = state.entries.find(key);
    if (map_it == state.entries.end() || !map_it->second.pinned) {
        return false;
    }

    state.unpinned.splice(state.unpinned.end(), state.pinned, map_it->second.it);
    map_it->second.pinned = false;
    map_it->second.it = std::prev(state.unpinned.end());
    return true;
}

void LRUCache::merge(LRUCache& other)
{
    if (this == &other) {
        return;
    }

    CacheState& state = State(this);
    CacheState& other_state = State(&other);

    for (auto it = other_state.pinned.begin(); it != other_state.pinned.end();) {
        auto current = it++;
        if (state.entries.find(current->first) != state.entries.end()) {
            continue;
        }
        state.pinned.splice(state.pinned.end(), other_state.pinned, current);
        state.entries[current->first] = {true, std::prev(state.pinned.end())};
    }

    for (auto it = other_state.unpinned.begin(); it != other_state.unpinned.end();) {
        auto current = it++;
        if (state.entries.find(current->first) != state.entries.end()) {
            continue;
        }
        state.unpinned.splice(state.unpinned.end(), other_state.unpinned, current);
        state.entries[current->first] = {false, std::prev(state.unpinned.end())};
    }

    other_state.pinned.clear();
    other_state.unpinned.clear();
    other_state.entries.clear();

    EvictIfNeeded(this);
}
