#include "../include/browser.hpp"
#include <string>
#include <thread>
#include <unistd.h>
#include "../include/site.hpp"
#include "../include/gemini-client.hpp"
#include "../include/utils.hpp"
#include <cstddef>
#include <cstdlib>
#include <optional>
#include <utility>
#include <vector>


void dispatch(std::vector<Link>* targets, Browser& b, int threadIdx) {
    std::vector<Link>& refT = *targets;
    for(int i =  0 ; i < refT.size() && i < SITE_CACHE_LIMIT; ++i) {
        auto& target = refT[i];
        b.justCacheSite(target);
    }
    b.setDone(threadIdx);
    delete targets;
}

void Browser::setDone(int threadIdx) {
    done[threadIdx] = true;
}

void Browser::tryCacheTargets() {
    bool dispatched = false;
    for(int i = 0; i < THREAD_NUM && dispatched == false; ++i) {
        if(done[i]) {
            if(threads[i].joinable()) {
                threads[i].join();
            }
            auto* lls = getLinkLines();
            done[i] = false;
            threads[i] = std::thread(dispatch, lls, std::ref(*this), i);
            dispatched = true;
        }
    }

}

Site* Browser::findInCacheAndPromoteIfRelevant(std::string& urlString) {
        Site*  site = nullptr;
        std::optional<Site> cachedSite = visitedCache->getSite(urlString);
        if(cachedSite != std::nullopt) {
            site = new Site(*cachedSite);
        }
        if(site == nullptr) {
            std::optional<Site> cached = preFetchCache->getSite(urlString);
            if(cached != std::nullopt) {
                site = new Site(*cached);
                visitedCache->addSite(urlString, *cached);
            }
        }

        return site;
}

void Browser::refresh() {
    goToSite(getPriorUri().value().to_string(), false, true);
}

void Browser::goToSite(std::string url, bool addToHistory, bool refresh) {

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

    std::string urlString = destination->getLinkDestination().to_string();
    std::string scheme  = destination->getLinkDestination().get_scheme();
    if(scheme != "gemini" && scheme != "file" && scheme != "about") { //  TODO: Should  I use about or just a fs file?
        openUrl(urlString);
        delete destination;
        return;
    }


    Site* site = nullptr;

    if(urlString.find("gemini://") != -1 && !refresh) {
        site = findInCacheAndPromoteIfRelevant(urlString);
    }
    
    if(site == nullptr) {
        site = client.fetchSite(*destination);
    }

    if(site == nullptr || site->getUnreachable()) {
        if(site != nullptr) {
            delete site;
        }
        previousIdx += 1;
        delete destination;
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

    if(urlString.find("gemini://") != -1) {
        visitedCache->addSite(urlString, *site);
    }

    for(auto* line: lines) {
        delete line;
    }

    lines = toLines(site);

    setLinksOfCurrentLines();
    previousStatusCodes[destination->getLinkDestination().to_string()] = site->getStatusCode();
    if (!addToHistory) {
        delete destination;
    }

    tryCacheTargets();
}

void Browser::justCacheSite(Link link) {
    auto client = GeminiClient{};

    std::string urlString = link.getLinkDestination().to_string();

    if (visitedCache->getSite(urlString) || preFetchCache->getSite(urlString)) {
        return;
    }

    Site* site = nullptr;

    if(urlString.find("gemini://") != -1) {
        site = client.fetchSite(link);
    } 

    if(site != nullptr) {
        preFetchCache->addSite(urlString, *site);
        delete site;
    }

    return;
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

    bool isPreformatted = false;
    for(auto line: lines) {
        res.push_back(lineToLine(line, getPriorUri(), lc, isPreformatted));
        if (res[res.size()-1]->type() == FORMAT_SWITCH) {
            isPreformatted = !isPreformatted;
        }
        if(res[res.size() - 1]->type() == LINK) {
            lc += 1;
        }
    }
    return res;
}


Browser::Browser() : threads(THREAD_NUM), done(THREAD_NUM) {
    currentSite = nullptr;
    visitedCache = new Cache{};
    preFetchCache = new Cache{};
    for (auto& d : done) {
        d = true;
    }

}

// TODO: SHould add more stuff here too, like the links stuff.
Browser::~Browser() {

    for (auto& t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }

    delete visitedCache;
    delete preFetchCache;
    delete currentSite;

    for (auto* line : lines) {
        delete line;
    }

    for (auto* link : siteHistory) {
        delete link;
    }
}


Site* Browser::getCurrentSite() {
    return currentSite;
}

std::vector<std::pair<std::string, TextRender>> Browser::renderSite() {
    std::vector<std::pair<std::string, TextRender>> res{};
    for(auto* line: lines) {
        std::pair<std::string,TextRender> cp {line->textToDraw(), TextRender {line->getColor(), line->isBold()}};
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

std::vector<Link>* Browser::getLinkLines() {
    std::vector<Link>* res = new std::vector<Link> {};
    for(auto& ln : links) {
        res->push_back(*dynamic_cast<Link*>(lines[ln]));
    }
    return res;
}


void Browser::followLinkNumber(int linkToFollow) {
    if(links.size() > linkToFollow-1 && linkToFollow-1 >= 0) {
        std::size_t pos = links[linkToFollow-1];
        if(lines.size() > pos && pos >= 0) {
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

Link* Browser::getCurrentLink() {
    if(previousIdx >= 0 && previousIdx < siteHistory.size()) {
        return this->siteHistory[previousIdx];
    }
    return nullptr;
}

