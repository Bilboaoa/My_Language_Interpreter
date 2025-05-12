#pragma once
#include "error.hpp"

class InterpreterException : public std::runtime_error
{
   public:
    Error error;

    explicit InterpreterException(const Error& e) : std::runtime_error(e.toString()), error(e) {}
    explicit InterpreterException(const ErrorType type, const std::string& message,
                                  const Position pos)
        : std::runtime_error(Error{type, message, pos}.toString()), error({type, message, pos})
    {
    }
};