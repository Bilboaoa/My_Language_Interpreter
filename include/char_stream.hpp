#pragma once
#include <istream>

class CharStream {
    std::istream& input;


public:
    CharStream(std::istream& in) : input(in) {}

    char get();

    char peek();

    bool eof() const; 
};
