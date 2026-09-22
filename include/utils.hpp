#pragma once
#include <optional>
#include <string>
#include <vector>
#include "line.hpp"
#include "../vendor/uri.hpp"

bool isPrefixed(std::string input, std::string prefix);
std::string readFileToString(std::string filePath);
bool isWhiteSpace(std::string& line, int idx);
std::string truncateAfter(std::string input, char truncate);
std::vector<std::string> stringToList(std::string input);
Line* lineToLine(std::string input, std::optional<uri> prior, int linkCount);
std::string urlEncode(const std::string& value);
