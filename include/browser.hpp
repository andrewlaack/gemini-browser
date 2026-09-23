#pragma once

#include <cstddef>
#include <unordered_map>
#include <vector>
#include "line.hpp"
#include "site.hpp"
#include "cache.hpp"
#include "link.hpp"
#include "utils.hpp"

class Browser {
    private:
        std::unordered_map<std::string, int> previousStatusCodes;
        std::vector<Link*> siteHistory;
        int previousIdx = -1;
        Cache* visitedCache;
        Cache* preFetchCache;
        Site* currentSite;
        std::vector<Line*> lines;
        std::vector<std::size_t> links; // these point to line indices
        void setLinksOfCurrentLines();
        Site* findInCacheAndPromoteIfRelevant(std::string& urlString);
    public:
        Browser();
        ~Browser();
        void goToSite(std::string url, bool addToHistory, bool refresh = false);
        void refresh();
        Site* getCurrentSite();
        Link* getCurrentLink();
        std::optional<uri> getPriorUri();
        std::vector<std::pair<std::string, TextRender>> renderSite();
        std::vector<Line*> toLines(Site* site);
        void followLinkNumber(int linkToFollow);
        std::vector<Link> getLinkLines();

        void justCacheSite(Link link);
        void goBack();
        void goForward();
};
