#include <fstream>
#include <iostream>
#include <variant>
#include "parser.hpp"

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
    Parser parser(lexer);

    std::unique_ptr<ProgramNode> program = parser.parseProgram();
    std::cout << program->toStr(0) << std::endl;
    return 0;
}