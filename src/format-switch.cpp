#include "../include/format-switch.hpp"
#include <ncurses.h>

FormatSwitch::FormatSwitch(std::string input) {
    text = input;
}

std::string FormatSwitch::textToDraw() {
    return text + "\n";
}

int FormatSwitch::getColor() {
    return COLOR_WHITE;
}

LineType FormatSwitch::type() {
    return FORMAT_SWITCH;
}

