#include "../include/browser.hpp"
#include "../include/site.hpp"
#include "../include/gemini-client.hpp"
#include "../include/utils.hpp"
#include <cstddef>
#include <optional>
#include <utility>
#include <vector>

void Browser::goToSite(std::string url, bool addToHistory) {

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

    Site* site = client.fetchSite(*destination);

    if(site == nullptr || site->getUnreachable()) {
        if(site != nullptr) {
            delete site;
        }
        goBack();
        return;
    }

    if(addToHistory) {
        while(siteHistory.size() > previousIdx + 1) {
            delete siteHistory[siteHistory.size() -  1];
            siteHistory.pop_back();
        }
        siteHistory.push_back(destination);
        previousIdx = siteHistory.size() - 1;
    }

    if(currentSite != nullptr) {
        delete currentSite;
    }
    currentSite = site;
    lines = toLines(site);
    setLinksOfCurrentLines();
    previousStatusCodes[destination->getLinkDestination().to_string()] = site->getStatusCode();
}

void Browser::setLinksOfCurrentLines() {
    links = std::vector<std::size_t> {};
    for(std::size_t i = 0; i < lines.size(); ++i) {
        if(lines[i]->type() == LINK) {
            links.push_back(i);
        }
    }
}


// TODO: This is a pure function.
std::vector<Line*> Browser::toLines(Site* site) {

    auto lines = stringToList(site->getBody());

    std::vector<Line*> res{};

    int lc = 1;

    for(auto line: lines) {
        res.push_back(lineToLine(line, getPriorUri(), lc));
        if(res[res.size() - 1]->type() == LINK) {
            lc += 1;
        }
    }
    return res;
}


Browser::Browser() {
    currentSite = nullptr;
}

Site* Browser::getCurrentSite() {
    return currentSite;
}

std::vector<std::pair<std::string, int>> Browser::renderSite() {
    std::vector<std::pair<std::string, int>> res{};
    for(auto* line: lines) {
        std::pair<std::string,int> cp {line->textToDraw(), line->getColor()};
        res.push_back(cp);
    }
    return res;
}


std::optional<uri> Browser::getPriorUri() {
    if(previousIdx < siteHistory.size() && previousIdx >= 0) {
        return siteHistory[previousIdx]->getLinkDestination();
    }
    return std::nullopt;

}

void Browser::followLinkNumber(int linkToFollow) {

    if(links.size() > linkToFollow-1 && linkToFollow-1 > 0) {
        std::size_t pos = links[linkToFollow-1];
        if(lines.size() > pos && pos > 0) {
            Line* ptr = lines[pos];
            Link* ptrLnk = dynamic_cast<Link*>(ptr);
            goToSite(ptrLnk->getLinkDestination().to_string(), true);
        }
    }

}

void Browser::goBack() {

    int original = previousIdx;

    previousIdx -= 1;

    if(siteHistory.size() > previousIdx && previousIdx >= 0) {

        int prSC = previousStatusCodes[siteHistory[previousIdx]->getLinkDestination().to_string()];
        while(!(prSC >= 20 && prSC <= 29)) {
            previousIdx -= 1;
            if(siteHistory.size() > previousIdx && previousIdx >= 0) {
                prSC = previousStatusCodes[siteHistory[previousIdx]->getLinkDestination().to_string()];
            } else {
                previousIdx = original;
                return;
            }
        }

        goToSite(this->siteHistory[previousIdx]->getLinkDestination().to_string(), false);

    } else {
        previousIdx = original;
    }
}

void Browser::goForward() {

    int original = previousIdx;

    previousIdx += 1;

    if(siteHistory.size() > previousIdx && previousIdx >= 0) {
        int prSC = previousStatusCodes[siteHistory[previousIdx]->getLinkDestination().to_string()];
        while(!(prSC >= 20 && prSC <= 29)) {
            previousIdx += 1;
            if(siteHistory.size() > previousIdx && previousIdx >= 0) {
                prSC = previousStatusCodes[siteHistory[previousIdx]->getLinkDestination().to_string()];
            } else {
                previousIdx = original;
                return;
            }
        }

        goToSite(this->siteHistory[previousIdx]->getLinkDestination().to_string(), false);
    } else {
        previousIdx = original;
    }
}
