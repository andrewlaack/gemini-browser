#include "../include/preformatted.hpp"
#include <ncurses.h>

Preformatted::Preformatted(std::string input) {
    text = input;
}

std::string Preformatted::textToDraw() {
    return text + "\n";
}

int Preformatted::getColor() {
    return COLOR_MAGENTA;
}

LineType Preformatted::type() {
    return PLAINTEXT;
}

