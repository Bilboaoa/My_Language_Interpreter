#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <optional>

enum class TokenType
{
    Identifier,
    Number,
    StringLiteral,
    Type,

    // Keywords
    Var,
    Const,
    Fun,
    Return,
    If,
    Else,
    While,
    As,
    Print,

    // Operators
    Plus,
    Minus,
    Star,
    Slash,
    Equal,
    EqualEqual,
    NotEqual,
    Greater,
    GreaterEqual,
    Less,
    LessEqual,
    Pipe,
    AtAt,
    Assign,
    And,
    Or,

    // Symbols
    LParen,
    RParen,
    LBracket,
    RBracket,
    Semicolon,
    Comma,

    EndOfFile,
    Unknown
};

struct Token
{
    TokenType type;
    std::optional<std::string> value;
    int line;
    int column;

    Token(TokenType t, const std::string& v, int l, int c) : type(t), value(v), line(l), column(c)
    {
    }
    Token(TokenType type, int line, int column)
        : type(type), value(std::nullopt), line(line), column(column) {}
};

class Lexer
{
   public:
    explicit Lexer(std::istream& inputStream);
    std::vector<Token> tokenize();

   private:
    std::istream& input;
    int line;
    int column;
    char currentChar;

    char get();
    void skipWhitespaceAndComments();
    Token scanToken();
};
