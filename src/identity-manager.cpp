#include <string>
#include <unordered_map>
#include "../vendor/uri.hpp"
#include "../include/identity-manager.hpp"

IdentityManager::IdentityManager(std::string configurationPath) {
    // TODO: Implement this.
    // lookup["bbs.geminispace.org"] = Identity{"/home/andrew/client.key", "/home/andrew/client.crt"};
}

Identity IdentityManager::getIdentityForURI(uri destination) {
    // TODO: Implement this. 
    return Identity{"",""};
}


