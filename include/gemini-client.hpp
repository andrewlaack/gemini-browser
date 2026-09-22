#pragma once

#include "site.hpp"
#include "link.hpp"

class GeminiClient {
        Site* getNetworkedSite(Link link);
    public:
        Site* fetchSite(Link link);
};
