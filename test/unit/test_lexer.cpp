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

TEST_CASE("Constant variable declaration", "[lexer]") {
    std::istringstream input("const var s = 12;");
    Lexer lexer(input);
    auto tokens = lexer.tokenize();

    REQUIRE(tokens[0].type == TokenType::Const);
    REQUIRE(tokens[0].value == "const");
    REQUIRE(tokens[1].type == TokenType::Var);
}

TEST_CASE("Function declaration", "[lexer]") {
    std::istringstream input("fun abc(var a) [return a;]");
    Lexer lexer(input);
    auto tokens = lexer.tokenize();

    REQUIRE(tokens[0].type == TokenType::Fun);
    REQUIRE(tokens[0].value == "fun");
    REQUIRE(tokens[1].type == TokenType::Identifier);
    REQUIRE(tokens[1].value == "abc");
    REQUIRE(tokens[2].type == TokenType::LParen);
    REQUIRE(tokens[3].type == TokenType::Var);
    REQUIRE(tokens[4].type == TokenType::Identifier);
    REQUIRE(tokens[4].value == "a");
    REQUIRE(tokens[5].type == TokenType::RParen);
    REQUIRE(tokens[6].type == TokenType::LBracket);
    REQUIRE(tokens[7].type == TokenType::Return);
    REQUIRE(tokens[8].type == TokenType::Identifier);
    REQUIRE(tokens[8].value == "a");
    REQUIRE(tokens[9].type == TokenType::Semicolon);
    REQUIRE(tokens[10].type == TokenType::RBracket);
}

TEST_CASE("Basic if statement", "[lexer]") {
    std::istringstream input("if(a > b)");
    Lexer lexer(input);
    auto tokens = lexer.tokenize();

    REQUIRE(tokens[0].type == TokenType::If);
    REQUIRE(tokens[1].type == TokenType::LParen);
    REQUIRE(tokens[2].type == TokenType::Identifier);
    REQUIRE(tokens[2].value == "a");
    REQUIRE(tokens[3].type == TokenType::Greater);
    REQUIRE(tokens[4].type == TokenType::Identifier);
    REQUIRE(tokens[4].value == "b");
    REQUIRE(tokens[5].type == TokenType::RParen);
}

TEST_CASE("Basic if else statement", "[lexer]") {
    std::istringstream input(R"(if(a > b)  
        [
        return 0; 
        ]
        else 
        [ 
        return 1; 
        ])");
    Lexer lexer(input);
    auto tokens = lexer.tokenize();

    REQUIRE(tokens[0].type == TokenType::If);
    REQUIRE(tokens[1].type == TokenType::LParen);
    REQUIRE(tokens[2].type == TokenType::Identifier);
    REQUIRE(tokens[2].value == "a");
    REQUIRE(tokens[3].type == TokenType::Greater);
    REQUIRE(tokens[4].type == TokenType::Identifier);
    REQUIRE(tokens[4].value == "b");
    REQUIRE(tokens[5].type == TokenType::RParen);
    REQUIRE(tokens[6].type == TokenType::LBracket);
    REQUIRE(tokens[7].type == TokenType::Return);
    REQUIRE(tokens[8].type == TokenType::Number);
    REQUIRE(tokens[8].value == "0");
    REQUIRE(tokens[9].type == TokenType::Semicolon);
    REQUIRE(tokens[10].type == TokenType::RBracket);
    REQUIRE(tokens[11].type == TokenType::Else);
    REQUIRE(tokens[12].type == TokenType::LBracket);
    REQUIRE(tokens[13].type == TokenType::Return);
    REQUIRE(tokens[14].type == TokenType::Number);
    REQUIRE(tokens[14].value == "1");
    REQUIRE(tokens[15].type == TokenType::Semicolon);
    REQUIRE(tokens[16].type == TokenType::RBracket);
}

TEST_CASE("Basic while statement", "[lexer]") {
    std::istringstream input(R"(while(a >= b)  
        [
        a = a + 1; 
        ]
        )");
    Lexer lexer(input);
    auto tokens = lexer.tokenize();

    REQUIRE(tokens[0].type == TokenType::While);
    REQUIRE(tokens[1].type == TokenType::LParen);
    REQUIRE(tokens[2].type == TokenType::Identifier);
    REQUIRE(tokens[2].value == "a");
    REQUIRE(tokens[3].type == TokenType::GreaterEqual);
    REQUIRE(tokens[4].type == TokenType::Identifier);
    REQUIRE(tokens[4].value == "b");
    REQUIRE(tokens[5].type == TokenType::RParen);
    REQUIRE(tokens[6].type == TokenType::LBracket);
    REQUIRE(tokens[7].type == TokenType::Identifier);
    REQUIRE(tokens[8].type == TokenType::Assign);
    REQUIRE(tokens[9].type == TokenType::Identifier);
    REQUIRE(tokens[10].type == TokenType::Plus);
    REQUIRE(tokens[11].type == TokenType::Number);
    REQUIRE(tokens[12].type == TokenType::Semicolon);
    REQUIRE(tokens[13].type == TokenType::RBracket);
}

TEST_CASE("Basic as statement", "[lexer]") {
    std::istringstream input(R"( my_var as float; )");
    Lexer lexer(input);
    auto tokens = lexer.tokenize();

    REQUIRE(tokens[0].type == TokenType::Identifier);
    REQUIRE(tokens[0].value == "my_var");
    REQUIRE(tokens[1].type == TokenType::As);
    REQUIRE(tokens[2].type == TokenType::Type);
    REQUIRE(tokens[3].type == TokenType::Semicolon);
}

TEST_CASE("Basic print", "[lexer]") {
    std::istringstream input(R"( print("Hello World"); )");
    Lexer lexer(input);
    auto tokens = lexer.tokenize();

    REQUIRE(tokens[0].type == TokenType::Print);
    REQUIRE(tokens[1].type == TokenType::LParen);
    REQUIRE(tokens[2].type == TokenType::StringLiteral);
    REQUIRE(tokens[2].value == "Hello World");
    REQUIRE(tokens[3].type == TokenType::RParen);
    REQUIRE(tokens[4].type == TokenType::Semicolon);
}

TEST_CASE("Operator Plus", "[lexer][operators]") {
    std::istringstream input("a + b;");
    Lexer lexer(input);
    auto tokens = lexer.tokenize();

    REQUIRE(tokens[0].type == TokenType::Identifier);
    REQUIRE(tokens[1].type == TokenType::Plus);
    REQUIRE(tokens[2].type == TokenType::Identifier);
    REQUIRE(tokens[3].type == TokenType::Semicolon);
}

TEST_CASE("Operator Minus", "[lexer][operators]") {
    std::istringstream input("a - b;");
    Lexer lexer(input);
    auto tokens = lexer.tokenize();

    REQUIRE(tokens[0].type == TokenType::Identifier);
    REQUIRE(tokens[1].type == TokenType::Minus);
    REQUIRE(tokens[2].type == TokenType::Identifier);
    REQUIRE(tokens[3].type == TokenType::Semicolon);
}

TEST_CASE("Operator Star", "[lexer][operators]") {
    std::istringstream input("a * b;");
    Lexer lexer(input);
    auto tokens = lexer.tokenize();

    REQUIRE(tokens[0].type == TokenType::Identifier);
    REQUIRE(tokens[1].type == TokenType::Star);
    REQUIRE(tokens[2].type == TokenType::Identifier);
    REQUIRE(tokens[3].type == TokenType::Semicolon);
}

TEST_CASE("Operator Slash", "[lexer][operators]") {
    std::istringstream input("a / b;");
    Lexer lexer(input);
    auto tokens = lexer.tokenize();

    REQUIRE(tokens[0].type == TokenType::Identifier);
    REQUIRE(tokens[1].type == TokenType::Slash);
    REQUIRE(tokens[2].type == TokenType::Identifier);
    REQUIRE(tokens[3].type == TokenType::Semicolon);
}

TEST_CASE("Operator Assign", "[lexer][operators]") {
    std::istringstream input("a = b;");
    Lexer lexer(input);
    auto tokens = lexer.tokenize();

    REQUIRE(tokens[0].type == TokenType::Identifier);
    REQUIRE(tokens[1].type == TokenType::Assign);
    REQUIRE(tokens[2].type == TokenType::Identifier);
    REQUIRE(tokens[3].type == TokenType::Semicolon);
}
