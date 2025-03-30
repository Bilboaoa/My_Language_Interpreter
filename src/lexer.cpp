#include "lexer.h"
#include <cctype>

Lexer::Lexer(std::istream& inputStream)
    : input(inputStream), line(1), column(0), currentChar(0), hasPeeked(false) {}

char Lexer::peek() {
    if (!hasPeeked) {
        currentChar = input.get();
        hasPeeked = true;
    }
    return currentChar;
}

char Lexer::get() {
    char ch;
    if (hasPeeked) {
        ch = currentChar;
        hasPeeked = false;
    } else {
        ch = input.get();
    }

    if (ch == '\n') {
        line++;
        column = 0;
    } else {
        column++;
    }

    return ch;
}

void Lexer::skipWhitespace() {
    while (std::isspace(peek())) {
        get();
    }
}

Token Lexer::scanToken() {
    skipWhitespace();
    char ch = peek();

    if (input.eof()) {
        return {TokenType::EndOfFile, "", line, column};
    }

    if (std::isalpha(ch) || ch == '_') {
        std::string ident;
        while (std::isalnum(peek()) || peek() == '_') {
            ident += get();
        }
        return {TokenType::Identifier, ident, line, column};
    }

    if (std::isdigit(ch)) {
        std::string number;
        while (std::isdigit(peek())) {
            number += get();
        }
        return {TokenType::Number, number, line, column};
    }

    // Unknown single-character token
    std::string unknown(1, get());
    return {TokenType::Unknown, unknown, line, column};
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    while (true) {
        Token token = scanToken();
        tokens.push_back(token);
        if (token.type == TokenType::EndOfFile)
            break;
    }
    return tokens;
}
