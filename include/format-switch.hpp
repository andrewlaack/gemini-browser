#pragma once
#include "line.hpp"

class FormatSwitch : public Line {
    private:
        std::string text = "";
    public:
        FormatSwitch(std::string text);
        std::string textToDraw() override;
        int getColor() override;
        LineType type() override;
};
