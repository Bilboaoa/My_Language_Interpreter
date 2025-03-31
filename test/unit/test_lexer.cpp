#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>
#include <sstream>
#include "lexer.h"

TEST_CASE("Simple variable declaration", "[lexer]") {
    std::istringstream input("var x = 42;");
    Lexer lexer(input);
    auto tokens = lexer.tokenize();

    REQUIRE(tokens[0].type == TokenType::Var);
    REQUIRE(tokens[1].type == TokenType::Identifier);
    REQUIRE(tokens[1].value == "x");
    REQUIRE(tokens[2].type == TokenType::Assign);
    REQUIRE(tokens[3].type == TokenType::Number);
    REQUIRE(tokens[3].value == "42");
    REQUIRE(tokens[4].type == TokenType::Semicolon);
    REQUIRE(tokens[5].type == TokenType::EndOfFile);
}

TEST_CASE("String literal parsing", "[lexer]") {
    std::istringstream input("var s = \"Hello\\nWorld\";");
    Lexer lexer(input);
    auto tokens = lexer.tokenize();

    REQUIRE(tokens[3].type == TokenType::StringLiteral);
    REQUIRE(tokens[3].value == "Hello\nWorld");
}
