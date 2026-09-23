#include "../include/cache.hpp"
#include "../include/site.hpp"
#include <cassert>
#include <mutex>
#include <optional>


std::optional<Site> Cache::getSite(const std::string& site) {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = cache.find(site);
    if (it != cache.end()) {
        return it->second;
    }
    return std::nullopt;
}

void Cache::addSite(std::string address, Site site, Importance importance) { // TODO: Use importance
    std::lock_guard<std::mutex> lock(mutex);
    assert(address.find("gemini://") != -1);
    cache.insert_or_assign(address,site);
}
