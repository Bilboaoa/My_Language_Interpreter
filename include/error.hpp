#pragma once
#include <string>
#include <stdexcept>

enum class ErrorType
{
    Lexical,
    Syntax,
    Semantic,
    Runtime
};

struct Error
{
    ErrorType type;
    std::string message;
    int line;
    int column;

    std::string toString() const
    {
        std::string typeStr;
        switch (type)
        {
            case ErrorType::Lexical:
                typeStr = "LexicalError";
                break;
            case ErrorType::Syntax:
                typeStr = "SyntaxError";
                break;
            case ErrorType::Semantic:
                typeStr = "SemanticError";
                break;
            case ErrorType::Runtime:
                typeStr = "RuntimeError";
                break;
        }

        return typeStr + " at " + std::to_string(line) + ":" + std::to_string(column) + " → " +
               message;
    }
};
