#pragma once

#include <optional>
#include <string>
#include "../vendor/uri.hpp"

class Link {
    private:
        uri linkDestination = uri("gemini://example.com");
        std::optional<std::string> linkText;
        uri parseDestination(std::string destination, std::optional<uri> prior);
    public:
        Link(std::string text, std::optional<uri> prior = std::nullopt);
        uri getLinkDestination();
        std::optional<std::string> getLinkText();
};
