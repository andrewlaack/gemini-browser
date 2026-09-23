// cache for prior sites and such. 
#include "site.hpp"
#include <optional>
#include <unordered_map>

class Cache {
    private:
        std::unordered_map<std::string,Site> cache;
    public:
        std::optional<Site> getSite(const std::string& site);
        void addSite(std::string address, Site site);
};
