#pragma once
#include "line.hpp"

class Preformatted : public Line {
    private:
        std::string text = "";
    public:
        Preformatted(std::string text);
        std::string textToDraw() override;
        int getColor() override;
        LineType type() override;
};
