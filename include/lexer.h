#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <optional>
#include <variant>
#include <limits>

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
    std::optional<std::variant<std::string, int, float>> value;
    int line;
    int column;

    Token(TokenType type, int line, int column)
        : type(type), value(std::nullopt), line(line), column(column) {}
    Token(TokenType t, const std::string& v, int l, int c) : type(t), value(v), line(l), column(c)
    {
    }
    Token(TokenType type, int val, int line, int column)
        : type(type), value(val), line(line), column(column) {}

    Token(TokenType type, float val, int line, int column)
        : type(type), value(val), line(line), column(column) {}

    template <typename T>
    T getValue() const {
        return std::get<T>(value.value());
    }
};

class Lexer
{
   public:
    explicit Lexer(std::istream& inputStream);
    std::vector<Token> tokenize();

   private:
    static constexpr int MAXINT = std::numeric_limits<int>::max();
    static constexpr int ASCII_ZERO = (int)0;

    std::istream& input;
    int line;
    int column;
    char currentChar;

    int digit_to_int(char digit) const;
    char get();
    void skipWhitespaceAndComments();
    Token scanToken();
};
