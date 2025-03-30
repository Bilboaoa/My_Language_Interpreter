#pragma once
#include <iostream>
#include <vector>
#include <string>

enum class TokenType {
    Identifier,
    Number,
    EndOfFile,
    Unknown
};

struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
};

class Lexer {
public:
    explicit Lexer(std::istream& inputStream);
    std::vector<Token> tokenize();

private:
    std::istream& input;
    int line;
    int column;
    int currentChar;
    bool hasPeeked;

    char peek();
    char get();
    void skipWhitespace();
    Token scanToken();
};
