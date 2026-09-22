#pragma once

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
    public:
        Browser();
        void goToSite(std::string url);
        Site* getCurrentSite();
        std::optional<uri> getPriorUri();
        std::string renderSite();
        std::vector<Line*> toLines(Site* site);
};
