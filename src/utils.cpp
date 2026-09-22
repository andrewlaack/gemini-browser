#include "../include/utils.hpp"
#include "../include/errors.hpp"
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

bool isPrefixed(std::string input, std::string prefix) {
    return std::string(input).find(prefix) == 0;
}

std::string readFileToString(std::string filePath) {
    auto in = std::ifstream(filePath);
    if(in.fail()) {
        throw FileReadError{};
    }
    std::ostringstream sstr;
    sstr << in.rdbuf();
    return sstr.str();
}

bool isWhiteSpace(std::string& line, int idx) {
    return line[idx] == ' ' || line[idx] == '\t';
}

std::string truncateAfter(std::string input, char truncate) {
    for(int i = input.size() - 1; i >= 0; --i) {
        if(input[i] == truncate) {
            return  input.substr(0,i+1);
        }
    }
    return input;
}
