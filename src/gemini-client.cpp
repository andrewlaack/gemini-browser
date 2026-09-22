#include "../include/gemini-client.hpp"
#include "../include/site.hpp"
#include "../include/utils.hpp"
#include "../include/errors.hpp"
#include <iostream>

Site* GeminiClient::fetchSite(Link link) {

    std::string destination = link.getLinkDestination().to_string();

    // TODO: Actually handle uris
    if(isPrefixed(destination, "gemini://")) {
        throw NotImplemented();
    } else if (isPrefixed(destination, "file://")){

        destination = destination.substr(7, destination.size() - 7);

        std::string fileStr = "";

        try {
            fileStr = readFileToString(destination);
        } catch (FileReadError e ) {
            return new Site {"51 \r\n", ""};
        }

        // TODO: how should I discern file types?
        return new Site {"20 text/gemini\r\n", fileStr};
    } else {
        throw NotImplemented();
    }

}
