#include "../include/site.hpp"
#include <iostream>
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

void Site::setUnreachable() {
    unreachable = true;
}

bool Site::getUnreachable() {
    return unreachable;
}

