#include "../include/link.hpp"
#include <iostream>
#include <optional>
#include <string>
#include "../include/utils.hpp"
#include "../vendor/uri.hpp"


Link::Link(std::string text, std::optional<uri> prior) {

    std::string afterPrefix = text;

    if(text.substr(0,2) == "=>") {
         afterPrefix = text.substr(2,text.size());
    }

    int highest = -1;

    for(int i = 0; i < afterPrefix.size(); ++i) {
        if(!(isWhiteSpace(afterPrefix, i))) {
            break;
        }
        highest = i;
    }

    std::string afterWhite = afterPrefix.substr(highest+1, afterPrefix.size());

    int nextWhite = std::min(afterWhite.find(' '), afterWhite.find('\t'));

    if(nextWhite != -1) {

        linkText = afterWhite.substr(nextWhite+1);
        
        std::string destination = afterWhite.substr(0, nextWhite);

        linkDestination = parseDestination(destination, prior);

    } else {
        linkDestination = parseDestination(afterWhite, prior);
    }
}

uri Link::parseDestination(std::string destination, std::optional<uri> prior) {

    // TODO: Not sure if this is right w/ how file paths work.
    if(destination.find("://") == -1) {
        if(prior != std::nullopt) {
            // TODO: Handle port
            std::string truncated = truncateAfter(prior->get_scheme() + "://" + prior->get_host() + "/" + prior->get_path(), '/');

            destination = truncated + destination;
        } 
        else {
            destination = "file://" + destination;
        }
    }
    try {
    return uri{destination};
    } catch(...) {
        invalid = true;
        return uri{"gemini://broken-link.com"}; // TODO: Handle this better
    }
}

uri Link::getLinkDestination() {
    return linkDestination;
}
std::optional<std::string> Link::getLinkText() {
    return linkText;
}

std::string Link::textToDraw() {
    if(linkText != std::nullopt) {
        return linkText.value() + "\n";
    }
    return linkDestination.to_string() + "\n";
}
