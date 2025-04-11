#include <fstream>
#include <iostream>
#include <variant>
#include "lexer.hpp"

struct PrintVisitor
{
    void operator()(std::monostate) const { std::cout << "null"; }
    void operator()(int val) const { std::cout << val; }
    void operator()(float val) const { std::cout << val; }
    void operator()(const std::string& val) const { std::cout << "\"" << val << "\""; }
};

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
        std::visit(PrintVisitor{}, t.value);

        std::cout << ", line: " << t.startPosition.line << ", col: " << t.startPosition.column
                  << ")\n";
    }

    return 0;
}