#pragma once
#include <string>
#include <variant>
#include "position.hpp"

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
    std::variant<std::monostate, std::string, int, float> value;
    Position startPosition;

    Token(TokenType type, Position pos) : type(type), value(std::monostate{}), startPosition(pos) {}

    Token(TokenType type, const std::string& val, Position pos)
        : type(type), value(val), startPosition(pos)
    {
    }
    Token(TokenType type, int val, Position pos) : type(type), value(val), startPosition(pos) {}

    Token(TokenType type, float val, Position start) : type(type), value(val), startPosition(start)
    {
    }

    template <typename T>
    T getValue() const
    {
        return std::get<T>(value);
    }
};