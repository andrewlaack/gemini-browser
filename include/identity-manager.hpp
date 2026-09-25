#pragma once

#include <string>
#include <unordered_map>
#include "../vendor/uri.hpp"

struct Identity {
    std::string keyPath = "";
    std::string crtPath = "";
};

class IdentityManager {
    private:
        // We want to search based on path first then domain.
        std::unordered_map<std::string, Identity> lookup;
    public:
        IdentityManager(std::string configurationPath);
        Identity getIdentityForURI(uri destination);
};
