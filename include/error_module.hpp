#include "error.hpp"

class InterpreterException : public std::runtime_error {
    public:
        Error error;
    
        InterpreterException(const Error& e)
            : std::runtime_error(e.toString()), error(e) {}
    };
    