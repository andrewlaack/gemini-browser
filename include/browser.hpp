#pragma once

#include <cstddef>
#include <vector>
#include "line.hpp"
#include "site.hpp"
#include "link.hpp"

class Browser {
    private:
        std::vector<Link*> siteHistory;
        int previousIdx = -1;
        Site* currentSite;
        std::vector<Line*> lines;
        std::vector<std::size_t> links; // these point to line indices
        void setLinksOfCurrentLines();
    public:
        Browser();
        void goToSite(std::string url, bool addToHistory);
        Site* getCurrentSite();
        std::optional<uri> getPriorUri();
        std::vector<std::pair<std::string, int>> renderSite();
        std::vector<Line*> toLines(Site* site);
        void followLinkNumber(int linkToFollow);
        void goBack();
        void goForward();
};
