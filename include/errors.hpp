#pragma once

#include <stdexcept>

class NotImplemented : public std::logic_error
{
public:
    NotImplemented() : std::logic_error("Function not yet implemented") { };
};


class FileReadError : public std::invalid_argument
{
public:
    FileReadError() : std::invalid_argument("Unable to read file") { };
};

