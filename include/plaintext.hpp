#pragma once
#include "line.hpp"

class Plaintext : public Line {
    private:
        std::string text = "";
    public:
        Plaintext(std::string text);
        std::string textToDraw() override;
        int getColor() override;
        LineType type() override;
};
