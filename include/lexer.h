#pragma once
#include <iostream>
#include <vector>
#include <string>

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
    std::string value;
    int line;
    int column;

    Token(TokenType t, const std::string& v, int l, int c) : type(t), value(v), line(l), column(c)
    {
    }
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
