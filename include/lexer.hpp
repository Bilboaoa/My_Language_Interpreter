#pragma once
#include <iostream>
#include <vector>
#include <limits>

#include "token.hpp"
#include "char_stream.hpp"

class Lexer
{
   public:
    explicit Lexer(CharStream& inputStream);
    Token scanToken();

   private:
    static constexpr int MAXINT = std::numeric_limits<int>::max();
    static constexpr int ASCII_ZERO = (int)'0';

    CharStream& input;
    int line;
    int column;
    char currentChar;

    int digit_to_int(char digit) const;
    char get();
    void skipWhitespaceAndComments();
};
