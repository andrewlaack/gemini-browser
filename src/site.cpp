#include "../include/site.hpp"
#include <algorithm>
#include <string>

Site::Site(std::string header, std::string body) {
    this->body = body;
    this->header = header;
}

std::string Site::getHeader() {
    return header;
}

std::string Site::getBody() {
    return body;
}
uint32_t Site::getStatusCode() {
    uint32_t statusCode = std::stoi(header.substr(0,2));
    return statusCode;
}

std::string Site::getMeta() {
    std::string meta = header.substr(3);
    std::replace(meta.begin(), meta.end(), '\n', ' ');
    return meta;
}


void Site::setUnreachable() {
    unreachable = true;
}

bool Site::getUnreachable() {
    return unreachable;
}

