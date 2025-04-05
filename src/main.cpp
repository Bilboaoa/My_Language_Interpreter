#include <fstream>
#include <iostream>
#include "lexer.hpp"

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        std::cerr << "Usage: ./bibl <filename>\n";
        return 1;
    }

    std::ifstream file(argv[1], std::ios::binary);
    if (!file.is_open())
    {
        std::cerr << "Failed to open file\n";
        return 1;
    }
    Lexer lexer(file);
    std::vector<Token> tokens;
    Token token = lexer.scanToken();

    while (token.type != TokenType::EndOfFile)
    {
        tokens.push_back(token);
        token = lexer.scanToken();
    }

    tokens.push_back(token);

    for (const auto& t : tokens)
{
    std::cout << "Token(" << static_cast<int>(t.type) << ", ";

    if (t.value.has_value()) {
        const auto& val = t.value.value();

        if (std::holds_alternative<int>(val)) {
            std::cout << std::get<int>(val);
        } else if (std::holds_alternative<float>(val)) {
            std::cout << std::get<float>(val);
        } else if (std::holds_alternative<std::string>(val)) {
            std::cout << "\"" << std::get<std::string>(val) << "\"";
        }
    } else {
        std::cout << "null";
    }

    std::cout << ", line: " << t.line << ", col: " << t.column << ")\n";
}


    return 0;
}
