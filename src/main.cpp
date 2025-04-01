#include <fstream>
#include <iostream>
#include "lexer.h"

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
    auto tokens = lexer.tokenize();

    for (const auto& t : tokens)
    {
        std::cout << "Token(" << static_cast<int>(t.type) << ", \"" << t.value
                  << "\", line: " << t.line << ", col: " << t.column << ")\n";
    }

    return 0;
}
