#include "../include/utils.hpp"
#include "../include/list-item.hpp"
#include "../include/quote.hpp"
#include "../include/heading.hpp"
#include "../include/link.hpp"
#include "../include/format-switch.hpp"
#include "../include/errors.hpp"
#include "../include/plaintext.hpp"
#include "../include/preformatted.hpp"
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <ios>
#include <iostream>
#include <linux/limits.h>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include <spawn.h>
#include <sys/wait.h>

extern char** environ;

void openUrl(const std::string& url) {
    pid_t pid;
    char* argv[] = {(char*)"xdg-open", (char*)url.c_str(), nullptr};
    if (posix_spawnp(&pid, "xdg-open", nullptr, nullptr, argv, environ) == 0) {
        waitpid(pid, nullptr, 0);
    }
}

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

Line* lineToLine(std::string input, std::optional<uri> prior, int linkCount, bool isPreformatted) {

    if(input.substr(0,3) == "```") {
        FormatSwitch* fs = new FormatSwitch{input};
        return fs;
    }

    if(!isPreformatted) {
        if(input.substr(0,2) == "=>") {
            Link* ln = new Link{input, prior, linkCount};
            return ln;
        }
        if(input.substr(0,1) == "#") {
            Heading* hd = new Heading{input};
            return hd;
        }

        if(input.substr(0,1) == ">") {
            Quote* qt= new Quote{input};
            return qt;
        }
        if(input.substr(0,1) == "*") {
            ListItem* li= new ListItem{input};
            return li;
        }


        return new Plaintext{input};
    } else {
        return new Preformatted{input};
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

// WIDTH IS INCLUSIVE
std::vector<std::pair<std::string, TextRender>> breakLines(std::vector<std::pair<std::string, TextRender>>& strLs, int width) {

    std::vector<std::pair<std::string, TextRender>> res {};

    for(std::size_t i = 0;  i < strLs.size(); ++i) {

        std::string cstr = strLs[i].first;
        std::string current = "";
        int lastSpace = -1;

        for(int x = 0; x < cstr.size(); ++x) {
            if(current.size() < width) {
                current.push_back(cstr[x]);
                if(cstr[x] == ' ') {
                    lastSpace = current.size()-1;
                }
            } else {

                std::string toPush = current;

                if(lastSpace != -1) {
                    toPush = current.substr(0,lastSpace+1);
                    current = current.substr(lastSpace+1);
                    current.push_back(cstr[x]);
                } else {
                    current = "";
                    current.push_back(cstr[x]);
                }

                res.push_back(std::pair<std::string,TextRender> {toPush,strLs[i].second});
                lastSpace = (cstr[x] == ' ') ? current.size() - 1 : -1;
            }
        }
        if(current.size() > 0) {
            res.push_back(std::pair<std::string,TextRender> {current,strLs[i].second});
            current = "";
        }
    }

    return res;
}
