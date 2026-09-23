#pragma once

#include <string>


enum LineType {
    LINK,
    PLAINTEXT,
    H1,
    H2,
    H3,
    FORMAT_SWITCH,
    PREFORMATTED
};

class Line {
    public:
        virtual std::string textToDraw() = 0;
        virtual int getColor() = 0;
        virtual LineType type() = 0;
        virtual ~Line() = default;
};
