#pragma once
#include <iostream>
#include <vector>
#include <limits>

#include "token.hpp"

class Lexer
{
   public:
    explicit Lexer(std::istream& inputStream);
    std::vector<Token> tokenize();

   private:
    static constexpr int MAXINT = std::numeric_limits<int>::max();
    static constexpr int ASCII_ZERO = (int)'0';

    std::istream& input;
    int line;
    int column;
    char currentChar;

    int digit_to_int(char digit) const;
    char get();
    void skipWhitespaceAndComments();
    Token scanToken();
};
