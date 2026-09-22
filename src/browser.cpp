#include "../include/browser.hpp"
#include "../include/site.hpp"
#include "../include/gemini-client.hpp"

void Browser::goToSite(std::string url) {

    Link* prior = nullptr;

    if(siteHistory.size() > previousIdx && previousIdx >= 0) {
        prior = siteHistory[previousIdx];
    }

    auto client = GeminiClient{};

    Link* destination = nullptr;

    if(prior == nullptr) {
        destination = new Link{url};
    } else {
        destination = new Link{url,prior->getLinkDestination()};
    }

    // TODO: Will need to clean up
    siteHistory.push_back(destination);
    previousIdx += 1;

    Site* site = client.fetchSite(*destination);

    if(currentSite != nullptr) {
        delete currentSite;
    }
    
    currentSite = site;
}

Browser::Browser() {
    currentSite = nullptr;
}

Site* Browser::getCurrentSite() {
    return currentSite;
}
