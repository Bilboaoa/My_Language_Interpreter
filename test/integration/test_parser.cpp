#include <memory>

#include "catch2/catch_all.hpp"

#include "asTree.hpp"
#include "parser.hpp"
#include "error.hpp"

class ParserTester {
public:
    std::istringstream stream;
    Lexer lexer;
    Parser parser;

    ParserTester(const std::string& input)
        : stream(input), lexer(stream), parser(lexer) {}
};

TEST_CASE("Minimal Declaration Test", "[parser][declaration]") {
    ParserTester parserTester("var x;");
    auto program = parserTester.parser.parseProgram();
    
    REQUIRE(program != nullptr);
    REQUIRE(program->declarations.size() == 1);

    std::string expected = "Var x;\n";
    std::string programString = program->toStr(0);
    REQUIRE(programString == expected);
}

TEST_CASE("Minimal function Declaration Test", "[parser][function]") {
    ParserTester parserTester("fun a() []");
    auto program = parserTester.parser.parseProgram();
    REQUIRE(program != nullptr);
    REQUIRE(program->declarations.size() == 1);

    std::string expected = "Fun a()\n [\n ]\n";
    std::string programString = program->toStr(0);
    REQUIRE(programString == expected);
}

TEST_CASE("Minimal function Declaration Test with body", "[parser][function][assign]") {
    ParserTester parserTester("fun a() [var a=2;]");
    auto program = parserTester.parser.parseProgram();
    REQUIRE(program != nullptr);
    REQUIRE(program->declarations.size() == 1);

    std::string expected = "Fun a()\n [\n  Var a = 2;\n ]\n";
    std::string programString = program->toStr(0);
    REQUIRE(programString == expected);
}

TEST_CASE("Test assign with float", "[parser][assign]") {
    ParserTester parserTester("var a=2.1;");
    auto program = parserTester.parser.parseProgram();
    REQUIRE(program != nullptr);
    REQUIRE(program->declarations.size() == 1);

    std::string expected = "Var a = 2.100000;\n";
    std::string programString = program->toStr(0);
    REQUIRE(programString == expected);
}

TEST_CASE("Test function declaration with return", "[parser][function][return]")
{
    ParserTester parserTester("fun a() [var a=2;return a;]");
    auto program = parserTester.parser.parseProgram();
    REQUIRE(program != nullptr);
    REQUIRE(program->declarations.size() == 1);

    std::string expected = "Fun a()\n [\n  Var a = 2;\n  return a;\n ]\n";
    std::string programString = program->toStr(0);
    REQUIRE(programString == expected);
}
// Funkcje protected -> dziedziczenie i konwersja na publiczne
// Lub publiczne -> We wlasciwym dziedziczenie po publicznych zmiana na prywantne
// Mozna wziac drzewo AST i konwersja na string po czym sprawdzac w testach
