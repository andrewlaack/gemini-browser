#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "link.hpp"

class Site{
    private:
        std::string body;
        std::vector<Line*> lines; 
        std::string header;
    public:
        Site(std::string header, std::string body);
        std::string getBody();
        uint32_t getStatusCode();
        std::string getHeader();
};
