#include "../include/utils.hpp"
#include "../include/link.hpp"
#include "../include/errors.hpp"
#include "../include/plaintext.hpp"
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

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

std::vector<std::string> stringToList(std::string input)
{
    std::vector<std::string> res;
    if (!input.empty()) {
        int start = 0;
        do {
            int idx = input.find('\n', start);
            if (idx == std::string::npos) {
                break;
            }
            int length = idx - start;
            res.push_back(input.substr(start, length));
            start += (length + 1);

        } while (true);
        res.push_back(input.substr(start));
    }

    return res;
}

Line* lineToLine(std::string input, std::optional<uri> prior, int linkCount) {
    if(input.substr(0,2) == "=>") {
        Link* ln = new Link{input, prior, linkCount};
        return ln;
    }
    return new Plaintext{input};
}
