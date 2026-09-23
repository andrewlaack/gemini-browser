#pragma once

#include <cstddef>
#include <unordered_map>
#include <vector>
#include "line.hpp"
#include "site.hpp"
#include "cache.hpp"
#include "link.hpp"

class Browser {
    private:
        std::unordered_map<std::string, int> previousStatusCodes;
        std::vector<Link*> siteHistory;
        int previousIdx = -1;
        Cache* cache;
        Site* currentSite;
        std::vector<Line*> lines;
        std::vector<std::size_t> links; // these point to line indices
        void setLinksOfCurrentLines();
    public:
        Browser();
        ~Browser();
        void goToSite(std::string url, bool addToHistory, bool refresh = false);
        void refresh();
        Site* getCurrentSite();
        std::optional<uri> getPriorUri();
        std::vector<std::pair<std::string, int>> renderSite();
        std::vector<Line*> toLines(Site* site);
        void followLinkNumber(int linkToFollow);
        void goBack();
        void goForward();
};
