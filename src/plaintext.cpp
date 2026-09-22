#include "../include/plaintext.hpp"

Plaintext::Plaintext(std::string input) {
    text = input;
}

std::string Plaintext::textToDraw() {
    return text + "\n";
}
