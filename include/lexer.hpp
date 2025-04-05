#pragma once
#include <iostream>
#include <vector>
#include <limits>

#include "token.hpp"

class Lexer
{
   public:
    explicit Lexer(std::istream& inputStream);
    Token scanToken();

   private:
    static constexpr int MAXINT = std::numeric_limits<int>::max();
    static constexpr int ASCII_ZERO = (int)'0';
    static constexpr int MAX_IDENTIFIER_LEN = 50;

    std::istream& input;
    int line;
    int column;
    char currentChar;

    int digit_to_int(char digit) const;
    char get();
    void skipWhitespaceAndComments();
    Token consumeAndReturn(Token returned);

    std::optional<Token> tryBuildIdentifier();
    std::optional<Token> tryBuildNumber();
    std::optional<Token> tryBuildString();
    std::optional<Token> tryBuildSymbol();
};
