#pragma once

#include <string>


enum LineType {
    LINK,
    PLAINTEXT
};

class Line {
    public:
        virtual std::string textToDraw() = 0;
        virtual int getColor() = 0;
        virtual LineType type() = 0;
};
