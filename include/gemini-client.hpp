#pragma once

#include "site.hpp"
#include "link.hpp"

class GeminiClient {
    public:
        Site* fetchSite(Link link);
};
