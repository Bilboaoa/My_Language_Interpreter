#include "lexer.h"
#include <cctype>
#include <unordered_map>

const std::unordered_map<std::string, TokenType> keywords = {
    {"var", TokenType::Var},
    {"const", TokenType::Const},
    {"fun", TokenType::Fun},
    {"return", TokenType::Return},
    {"if", TokenType::If},
    {"else", TokenType::Else},
    {"while", TokenType::While},
    {"as", TokenType::As},
    {"print", TokenType::Print}
};

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

void Lexer::skipWhitespaceAndComments() {
    char ch = peek();

    while (std::isspace(ch) || (ch == '/' && input.peek() == '/')) {
        if (std::isspace(ch)) {
            get();
        }

        else if (ch == '/') {
            get();
            if (peek() == '/') {
                get();
                while (peek() != '\n' && !input.eof()) get();
            }
        }

        ch = peek();
    }
}


Token Lexer::scanToken() {
    skipWhitespaceAndComments();
    if (input.eof()) return Token(TokenType::EndOfFile, "", line, column);

    char ch = peek();

    if (std::isalpha(ch) || ch == '_') {
        std::string ident;
        while (std::isalnum(peek()) || peek() == '_') {
            ident += get();
        }
        if (keywords.count(ident)) {
            return Token(keywords.at(ident), ident, line, column);
        }
        return Token(TokenType::Identifier, ident, line, column);
    }

    if (std::isdigit(ch)) {
        std::string number;
        while (std::isdigit(peek())) number += get();

        if (peek() == '.') {
            number += get();
            while (std::isdigit(peek())) number += get();
        }

        return Token(TokenType::Number, number, line, column);
    }

    if (ch == '"') {
        get();
        std::string str;
        while (peek() != '"' && !input.eof()) {
            if (peek() == '\\') {
                get(); 
                char esc = get();
                switch (esc) {
                    case 'n': str += '\n'; break;
                    case 't': str += '\t'; break;
                    case '"': str += '"'; break;
                    case '\\': str += '\\'; break;
                    default: str += esc; break;
                }
            } else {
                str += get();
            }
        }
        if (peek() == '"') get();
        return Token(TokenType::StringLiteral, str, line, column);
    }

    switch (get()) {
        case '+': return Token(TokenType::Plus, "+", line, column);
        case '-': return Token(TokenType::Minus, "-", line, column);
        case '*': return Token(TokenType::Star, "*", line, column);
        case '/': return Token(TokenType::Slash, "/", line, column);
        case '=':
            if (peek() == '=') { get(); return Token(TokenType::EqualEqual, "==", line, column); }
            return Token(TokenType::Assign, "=", line, column);
        case '!':
            if (peek() == '=') { get(); return Token(TokenType::NotEqual, "!=", line, column); }
            break;
        case '>':
            if (peek() == '=') { get(); return Token(TokenType::GreaterEqual, ">=", line, column); }
            return Token(TokenType::Greater, ">", line, column);
        case '<':
            if (peek() == '=') { get(); return Token(TokenType::LessEqual, "<=", line, column); }
            return Token(TokenType::Less, "<", line, column);
        case '|': return Token(TokenType::Pipe, "|", line, column);
        case '@':
            if (peek() == '@') { get(); return Token(TokenType::AtAt, "@@", line, column); }
            break;
        case '(': return Token(TokenType::LParen, "(", line, column);
        case ')': return Token(TokenType::RParen, ")", line, column);
        case '[': return Token(TokenType::LBracket, "[", line, column);
        case ']': return Token(TokenType::RBracket, "]", line, column);
        case ';': return Token(TokenType::Semicolon, ";", line, column);
        case ',': return Token(TokenType::Comma, ",", line, column);
    }

    return Token(TokenType::Unknown, std::string(1, ch), line, column);
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
