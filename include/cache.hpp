// cache for prior sites and such. 
#include "site.hpp"
#include <mutex>
#include <deque>
#include <optional>
#include <unordered_map>

enum Importance {
    IMPORTANT,
    NOT_IMPORTANT
};

class Cache {
    private:
        std::unordered_map<std::string,Site> cache;
        std::mutex mutex;
    public:
        std::optional<Site> getSite(const std::string& site);
        void addSite(std::string address, Site site, Importance importance = IMPORTANT);
};
