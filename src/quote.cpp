#include "../include/quote.hpp"
#include <ncurses.h>

Quote::Quote(std::string input) {
    text = input;
}

std::string Quote::textToDraw() {
    return text + "\n";
}

int Quote::getColor() {
    return COLOR_MAGENTA;
}

LineType Quote::type() {
    return QUOTE;
}

bool Quote::isBold() {
    return false;
}
