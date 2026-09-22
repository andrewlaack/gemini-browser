#pragma once
#include <string>
#include <vector>

bool isPrefixed(std::string input, std::string prefix);
std::string readFileToString(std::string filePath);
bool isWhiteSpace(std::string& line, int idx);
std::string truncateAfter(std::string input, char truncate);
std::vector<std::string> stringToList(std::string input);
