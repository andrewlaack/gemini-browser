#include "../include/utils.hpp"
#include "../include/link.hpp"
#include "../include/errors.hpp"
#include "../include/plaintext.hpp"
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <ios>
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


std::string urlEncode(const std::string& value) {
    std::ostringstream escaped;
    escaped.fill('0');
    escaped << std::hex;

    for (std::string::const_iterator i = value.begin(), n = value.end(); i != n; ++i) {
        std::string::value_type c = (*i);

        // Keep alphanumeric and other accepted characters intact
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            escaped << c;
            continue;
        }

        // Any other characters are percent-encoded
        escaped << std::uppercase;
        escaped << '%' << std::setw(2) << int((unsigned char) c);
        escaped << std::nouppercase;
    }

    return escaped.str();
}

std::string getNewTab() {
    std::string st = 
        "# New Tab\n"
        "\n"
        "This is a new tab. We have a few keybindings around here:\n"
        "\n"
        "* b -> back a page\n"
        "* f -> forward a page\n"
        "* o -> show url entry / link selection\n"
        ;
    return st;
}
