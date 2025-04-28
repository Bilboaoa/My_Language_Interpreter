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

TEST_CASE("Minimal Declaration Test", "[parser]") {
    ParserTester parserTester("var x;");
    auto program = parserTester.parser.parseProgram();
    REQUIRE(program != nullptr);
    REQUIRE(program->declarations.size() == 1);
}

TEST_CASE("Minimal function Declaration Test", "[parser]") {
    ParserTester parserTester("fun a() []");
    auto program = parserTester.parser.parseProgram();
    REQUIRE(program != nullptr);
    REQUIRE(program->declarations.size() == 1);
}

// Funkcje protected -> dziedziczenie i konwersja na publiczne
// Lub publiczne -> We wlasciwym dziedziczenie po publicznych zmiana na prywantne
// Mozna wziac drzewo AST i konwersja na string po czym sprawdzac w testach
