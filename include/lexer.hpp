#pragma once
#include <iostream>
#include <vector>
#include <limits>

#include "token.hpp"
#include "interpreter_exception.hpp"
#include "position.hpp"

class Lexer
{
   public:
    explicit Lexer(std::istream& inputStream);
    Token scanToken();

   private:
    static constexpr int MAXINT = std::numeric_limits<int>::max();
    static constexpr int MAX_IDENTIFIER_LEN = 50;

    std::istream& input;
    Position currentPosition;
    char currentChar;

    char get();
    void skipWhitespaceAndComments();
    Token consumeAndReturn(Token returned);
    void throwError(ErrorType type, const std::string& msg, Position pos) const;

    std::optional<Token> tryBuildIdentifier();
    std::optional<Token> tryBuildNumber();
    std::optional<Token> tryBuildString();
    std::optional<Token> tryBuildSymbol();
};
