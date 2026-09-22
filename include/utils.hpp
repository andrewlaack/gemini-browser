#pragma once
#include <string>

bool isPrefixed(std::string input, std::string prefix);
std::string readFileToString(std::string filePath);
bool isWhiteSpace(std::string& line, int idx);
std::string truncateAfter(std::string input, char truncate);
