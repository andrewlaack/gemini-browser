#pragma once

#include <vector>
#include "site.hpp"
#include "link.hpp"

class Browser {
    private:
        std::vector<Link*> siteHistory;
        int previousIdx = -1;
        Site* currentSite;
    public:
        Browser();
        void goToSite(std::string url);
        Site* getCurrentSite();
};
