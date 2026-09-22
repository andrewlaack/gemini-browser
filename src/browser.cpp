#include "../include/browser.hpp"
#include "../include/site.hpp"
#include "../include/gemini-client.hpp"
#include "../include/utils.hpp"
#include <iostream>
#include <optional>

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
    lines = toLines(site);
}

// TODO: This is a pure function.
std::vector<Line*> Browser::toLines(Site* site) {
    auto lines = stringToList(site->getBody());

    std::vector<Line*> res{};

    for(auto line: lines) {
        res.push_back(lineToLine(line, getPriorUri()));
    }
    return res;
}


Browser::Browser() {
    currentSite = nullptr;
}

Site* Browser::getCurrentSite() {
    return currentSite;
}

std::string Browser::renderSite() {
    std::string res = "";
    for(auto* line: lines) {
        res += line->textToDraw();
    }
    return res;
}


std::optional<uri> Browser::getPriorUri() {
    if(previousIdx < siteHistory.size() && previousIdx >= 0) {
        return siteHistory[previousIdx]->getLinkDestination();
    }
    return std::nullopt;

}

