#include "../include/cache.hpp"
#include "../include/site.hpp"
#include <cassert>
#include <optional>

std::optional<Site> Cache::getSite(const std::string& site) {
    auto it = cache.find(site);
    if (it != cache.end()) {
        return it->second;
    }
    return std::nullopt;
}

void Cache::addSite(std::string address, Site site) {
    assert(address.find("gemini://") != -1);
    cache.insert_or_assign(address,site);
}
