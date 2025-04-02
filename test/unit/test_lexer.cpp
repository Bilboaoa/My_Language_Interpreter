#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>
#include <sstream>
#include "lexer.hpp"

template <typename T>
T get_value(const Token& token)
{
    return std::get<T>(token.value.value());
}

TEST_CASE("Identifier parsing", "[lexer][identifier]")
{
    std::istringstream input("alpha _temp1 x2 alpha_temp1;");
    Lexer lexer(input);
    auto tokens = lexer.tokenize();

    REQUIRE(tokens[0].type == TokenType::Identifier);
    REQUIRE(tokens[0].getValue<std::string>() == "alpha");
    REQUIRE(tokens[1].type == TokenType::Identifier);
    REQUIRE(tokens[1].getValue<std::string>() == "_temp1");
    REQUIRE(tokens[2].type == TokenType::Identifier);
    REQUIRE(tokens[2].getValue<std::string>() == "x2");
    REQUIRE(tokens[3].type == TokenType::Identifier);
    REQUIRE(tokens[3].getValue<std::string>() == "alpha_temp1");
}

TEST_CASE("Invalid identifier parsing", "[lexer][identifier]")
{
    std::istringstream input("123xdd;");
    Lexer lexer(input);
    auto tokens = lexer.tokenize();

    REQUIRE(tokens[0].type == TokenType::Number);
    REQUIRE(tokens[0].getValue<int>() == 123);
    REQUIRE(tokens[1].type == TokenType::Identifier);
    REQUIRE(tokens[1].getValue<std::string>() == "xdd");
}

TEST_CASE("Identifier parsing with keyword", "[lexer][identifier]")
{
    std::istringstream input("var_a fun_a if_a");
    Lexer lexer(input);
    auto tokens = lexer.tokenize();

    REQUIRE(tokens[0].type == TokenType::Identifier);
    REQUIRE(tokens[0].getValue<std::string>() == "var_a");
    REQUIRE(tokens[1].type == TokenType::Identifier);
    REQUIRE(tokens[1].getValue<std::string>() == "fun_a");
    REQUIRE(tokens[2].type == TokenType::Identifier);
    REQUIRE(tokens[2].getValue<std::string>() == "if_a");
}

TEST_CASE("Number parsing (int)", "[lexer][number]")
{
    std::istringstream input("123;");
    Lexer lexer(input);
    auto tokens = lexer.tokenize();

    REQUIRE(tokens[0].type == TokenType::Number);
    REQUIRE(tokens[0].getValue<int>() == 123);
}

TEST_CASE("Number parsing (float)", "[lexer][number]")
{
    std::istringstream input("45.67;");
    Lexer lexer(input);
    auto tokens = lexer.tokenize();

    REQUIRE(tokens[0].type == TokenType::Number);
    REQUIRE(tokens[0].getValue<float>() == 45.67f);
}

TEST_CASE("String parsing", "[lexer][string]")
{
    std::istringstream input("\"Hi\"");
    Lexer lexer(input);
    auto tokens = lexer.tokenize();

    REQUIRE(tokens[0].type == TokenType::StringLiteral);
    REQUIRE(std::get<std::string>(tokens[0].value.value()) == "Hi");
}

TEST_CASE("String with escape characters", "[lexer][string]")
{
    std::istringstream input(R"(var s = "\"quoted\" and \\slash";)");
    Lexer lexer(input);
    auto tokens = lexer.tokenize();

    REQUIRE(tokens[3].type == TokenType::StringLiteral);
    REQUIRE(std::get<std::string>(tokens[3].value.value()) == "\"quoted\" and \\slash");
}

TEST_CASE("Simple variable declaration", "[lexer][keyword]")
{
    std::istringstream input("var x = 42;");
    Lexer lexer(input);
    auto tokens = lexer.tokenize();

    REQUIRE(tokens[0].type == TokenType::Var);
    REQUIRE(tokens[1].type == TokenType::Identifier);
    REQUIRE(tokens[1].getValue<std::string>() == "x");
    REQUIRE(tokens[2].type == TokenType::Assign);
    REQUIRE(tokens[3].type == TokenType::Number);
    REQUIRE(tokens[3].getValue<int>() == 42);
    REQUIRE(tokens[4].type == TokenType::Semicolon);
    REQUIRE(tokens[5].type == TokenType::EndOfFile);
}

TEST_CASE("String literal parsing", "[lexer][keyword]")
{
    std::istringstream input("var s = \"Hello\\nWorld\";");
    Lexer lexer(input);
    auto tokens = lexer.tokenize();

    REQUIRE(tokens[3].type == TokenType::StringLiteral);
    REQUIRE(tokens[3].getValue<std::string>() == "Hello\nWorld");
}

TEST_CASE("Type keywords recognized as TokenType::Type", "[lexer][type]")
{
    std::istringstream input("int float string");
    Lexer lexer(input);
    auto tokens = lexer.tokenize();

    REQUIRE(tokens[0].type == TokenType::Type);
    REQUIRE(tokens[0].getValue<std::string>() == "int");
    REQUIRE(tokens[1].type == TokenType::Type);
    REQUIRE(tokens[1].getValue<std::string>() == "float");
    REQUIRE(tokens[2].type == TokenType::Type);
    REQUIRE(tokens[2].getValue<std::string>() == "string");
}

TEST_CASE("Constant variable declaration", "[lexer][keyword]")
{
    std::istringstream input("const var s = 12;");
    Lexer lexer(input);
    auto tokens = lexer.tokenize();

    REQUIRE(tokens[0].type == TokenType::Const);
    REQUIRE(tokens[0].getValue<std::string>() == "const");
    REQUIRE(tokens[1].type == TokenType::Var);
}

TEST_CASE("Function declaration", "[lexer][keyword]")
{
    std::istringstream input("fun abc(var a) [return a;]");
    Lexer lexer(input);
    auto tokens = lexer.tokenize();

    REQUIRE(tokens[0].type == TokenType::Fun);
    REQUIRE(tokens[0].getValue<std::string>() == "fun");
    REQUIRE(tokens[1].type == TokenType::Identifier);
    REQUIRE(tokens[1].getValue<std::string>() == "abc");
    REQUIRE(tokens[2].type == TokenType::LParen);
    REQUIRE(tokens[3].type == TokenType::Var);
    REQUIRE(tokens[4].type == TokenType::Identifier);
    REQUIRE(tokens[4].getValue<std::string>() == "a");
    REQUIRE(tokens[5].type == TokenType::RParen);
    REQUIRE(tokens[6].type == TokenType::LBracket);
    REQUIRE(tokens[7].type == TokenType::Return);
    REQUIRE(tokens[8].type == TokenType::Identifier);
    REQUIRE(tokens[8].getValue<std::string>() == "a");
    REQUIRE(tokens[9].type == TokenType::Semicolon);
    REQUIRE(tokens[10].type == TokenType::RBracket);
}

TEST_CASE("Embeded function declaration", "[lexer][keyword]")
{
    std::istringstream input("fun abc(var a) [return fun(a) [return a+1; ];]");
    Lexer lexer(input);
    auto tokens = lexer.tokenize();

    REQUIRE(tokens[0].type == TokenType::Fun);
    REQUIRE(tokens[0].getValue<std::string>() == "fun");
    REQUIRE(tokens[1].type == TokenType::Identifier);
    REQUIRE(tokens[1].getValue<std::string>() == "abc");
    REQUIRE(tokens[2].type == TokenType::LParen);
    REQUIRE(tokens[3].type == TokenType::Var);
    REQUIRE(tokens[4].type == TokenType::Identifier);
    REQUIRE(tokens[4].getValue<std::string>() == "a");
    REQUIRE(tokens[5].type == TokenType::RParen);
    REQUIRE(tokens[6].type == TokenType::LBracket);
    REQUIRE(tokens[7].type == TokenType::Return);
    REQUIRE(tokens[8].type == TokenType::Fun);
    REQUIRE(tokens[9].type == TokenType::LParen);
    REQUIRE(tokens[10].type == TokenType::Identifier);
    REQUIRE(tokens[11].type == TokenType::RParen);
    REQUIRE(tokens[12].type == TokenType::LBracket);
    REQUIRE(tokens[13].type == TokenType::Return);
    REQUIRE(tokens[14].type == TokenType::Identifier);
    REQUIRE(tokens[14].getValue<std::string>() == "a");
    REQUIRE(tokens[15].type == TokenType::Plus);
    REQUIRE(tokens[16].type == TokenType::Number);
    REQUIRE(tokens[17].type == TokenType::Semicolon);
    REQUIRE(tokens[18].type == TokenType::RBracket);
    REQUIRE(tokens[19].type == TokenType::Semicolon);
    REQUIRE(tokens[20].type == TokenType::RBracket);
}

TEST_CASE("Basic if statement", "[lexer][keyword]")
{
    std::istringstream input("if(a > b)");
    Lexer lexer(input);
    auto tokens = lexer.tokenize();

    REQUIRE(tokens[0].type == TokenType::If);
    REQUIRE(tokens[1].type == TokenType::LParen);
    REQUIRE(tokens[2].type == TokenType::Identifier);
    REQUIRE(tokens[2].getValue<std::string>() == "a");
    REQUIRE(tokens[3].type == TokenType::Greater);
    REQUIRE(tokens[4].type == TokenType::Identifier);
    REQUIRE(tokens[4].getValue<std::string>() == "b");
    REQUIRE(tokens[5].type == TokenType::RParen);
}

TEST_CASE("Basic if else statement", "[lexer][keyword]")
{
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
    REQUIRE(tokens[2].getValue<std::string>() == "a");
    REQUIRE(tokens[3].type == TokenType::Greater);
    REQUIRE(tokens[4].type == TokenType::Identifier);
    REQUIRE(tokens[4].getValue<std::string>() == "b");
    REQUIRE(tokens[5].type == TokenType::RParen);
    REQUIRE(tokens[6].type == TokenType::LBracket);
    REQUIRE(tokens[7].type == TokenType::Return);
    REQUIRE(tokens[8].type == TokenType::Number);
    REQUIRE(tokens[8].getValue<int>() == 0);
    REQUIRE(tokens[9].type == TokenType::Semicolon);
    REQUIRE(tokens[10].type == TokenType::RBracket);
    REQUIRE(tokens[11].type == TokenType::Else);
    REQUIRE(tokens[12].type == TokenType::LBracket);
    REQUIRE(tokens[13].type == TokenType::Return);
    REQUIRE(tokens[14].type == TokenType::Number);
    REQUIRE(tokens[14].getValue<int>() == 1);
    REQUIRE(tokens[15].type == TokenType::Semicolon);
    REQUIRE(tokens[16].type == TokenType::RBracket);
}

TEST_CASE("Basic while statement", "[lexer][keyword]")
{
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
    REQUIRE(tokens[2].getValue<std::string>() == "a");
    REQUIRE(tokens[3].type == TokenType::GreaterEqual);
    REQUIRE(tokens[4].type == TokenType::Identifier);
    REQUIRE(tokens[4].getValue<std::string>() == "b");
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

TEST_CASE("Basic as statement", "[lexer][keyword]")
{
    std::istringstream input(R"( my_var as float; )");
    Lexer lexer(input);
    auto tokens = lexer.tokenize();

    REQUIRE(tokens[0].type == TokenType::Identifier);
    REQUIRE(tokens[0].getValue<std::string>() == "my_var");
    REQUIRE(tokens[1].type == TokenType::As);
    REQUIRE(tokens[2].type == TokenType::Type);
    REQUIRE(tokens[3].type == TokenType::Semicolon);
}

TEST_CASE("Basic print", "[lexer][keyword]")
{
    std::istringstream input(R"( print("Hello World"); )");
    Lexer lexer(input);
    auto tokens = lexer.tokenize();

    REQUIRE(tokens[0].type == TokenType::Print);
    REQUIRE(tokens[1].type == TokenType::LParen);
    REQUIRE(tokens[2].type == TokenType::StringLiteral);
    REQUIRE(tokens[2].getValue<std::string>() == "Hello World");
    REQUIRE(tokens[3].type == TokenType::RParen);
    REQUIRE(tokens[4].type == TokenType::Semicolon);
}

TEST_CASE("Operator Plus", "[lexer][operators]")
{
    std::istringstream input("a + b;");
    Lexer lexer(input);
    auto tokens = lexer.tokenize();

    REQUIRE(tokens[0].type == TokenType::Identifier);
    REQUIRE(tokens[1].type == TokenType::Plus);
    REQUIRE(tokens[2].type == TokenType::Identifier);
    REQUIRE(tokens[3].type == TokenType::Semicolon);
}

TEST_CASE("Operator Minus", "[lexer][operators]")
{
    std::istringstream input("a - b;");
    Lexer lexer(input);
    auto tokens = lexer.tokenize();

    REQUIRE(tokens[0].type == TokenType::Identifier);
    REQUIRE(tokens[1].type == TokenType::Minus);
    REQUIRE(tokens[2].type == TokenType::Identifier);
    REQUIRE(tokens[3].type == TokenType::Semicolon);
}

TEST_CASE("Operator Star", "[lexer][operators]")
{
    std::istringstream input("a * b;");
    Lexer lexer(input);
    auto tokens = lexer.tokenize();

    REQUIRE(tokens[0].type == TokenType::Identifier);
    REQUIRE(tokens[1].type == TokenType::Star);
    REQUIRE(tokens[2].type == TokenType::Identifier);
    REQUIRE(tokens[3].type == TokenType::Semicolon);
}

TEST_CASE("Operator Slash", "[lexer][operators]")
{
    std::istringstream input("a / b;");
    Lexer lexer(input);
    auto tokens = lexer.tokenize();

    REQUIRE(tokens[0].type == TokenType::Identifier);
    REQUIRE(tokens[1].type == TokenType::Slash);
    REQUIRE(tokens[2].type == TokenType::Identifier);
    REQUIRE(tokens[3].type == TokenType::Semicolon);
}

TEST_CASE("Operator Assign", "[lexer][operators]")
{
    std::istringstream input("a = b;");
    Lexer lexer(input);
    auto tokens = lexer.tokenize();

    REQUIRE(tokens[0].type == TokenType::Identifier);
    REQUIRE(tokens[1].type == TokenType::Assign);
    REQUIRE(tokens[2].type == TokenType::Identifier);
    REQUIRE(tokens[3].type == TokenType::Semicolon);
}

TEST_CASE("Operator EqualEqual", "[lexer][operators]")
{
    std::istringstream input("a == b;");
    Lexer lexer(input);
    auto tokens = lexer.tokenize();

    REQUIRE(tokens[0].type == TokenType::Identifier);
    REQUIRE(tokens[1].type == TokenType::EqualEqual);
    REQUIRE(tokens[2].type == TokenType::Identifier);
    REQUIRE(tokens[3].type == TokenType::Semicolon);
}

TEST_CASE("Operator NotEqual", "[lexer][operators]")
{
    std::istringstream input("a != b;");
    Lexer lexer(input);
    auto tokens = lexer.tokenize();

    REQUIRE(tokens[0].type == TokenType::Identifier);
    REQUIRE(tokens[1].type == TokenType::NotEqual);
    REQUIRE(tokens[2].type == TokenType::Identifier);
    REQUIRE(tokens[3].type == TokenType::Semicolon);
}

TEST_CASE("Operator Greater", "[lexer][operators]")
{
    std::istringstream input("a > b;");
    Lexer lexer(input);
    auto tokens = lexer.tokenize();

    REQUIRE(tokens[0].type == TokenType::Identifier);
    REQUIRE(tokens[1].type == TokenType::Greater);
    REQUIRE(tokens[2].type == TokenType::Identifier);
    REQUIRE(tokens[3].type == TokenType::Semicolon);
}

TEST_CASE("Operator GreaterEqual", "[lexer][operators]")
{
    std::istringstream input("a >= b;");
    Lexer lexer(input);
    auto tokens = lexer.tokenize();

    REQUIRE(tokens[0].type == TokenType::Identifier);
    REQUIRE(tokens[1].type == TokenType::GreaterEqual);
    REQUIRE(tokens[2].type == TokenType::Identifier);
    REQUIRE(tokens[3].type == TokenType::Semicolon);
}

TEST_CASE("Operator Less", "[lexer][operators]")
{
    std::istringstream input("a < b;");
    Lexer lexer(input);
    auto tokens = lexer.tokenize();

    REQUIRE(tokens[0].type == TokenType::Identifier);
    REQUIRE(tokens[1].type == TokenType::Less);
    REQUIRE(tokens[2].type == TokenType::Identifier);
    REQUIRE(tokens[3].type == TokenType::Semicolon);
}

TEST_CASE("Operator LessEqual", "[lexer][operators]")
{
    std::istringstream input("a <= b;");
    Lexer lexer(input);
    auto tokens = lexer.tokenize();

    REQUIRE(tokens[0].type == TokenType::Identifier);
    REQUIRE(tokens[1].type == TokenType::LessEqual);
    REQUIRE(tokens[2].type == TokenType::Identifier);
    REQUIRE(tokens[3].type == TokenType::Semicolon);
}

TEST_CASE("Operator AtAt", "[lexer][operators]")
{
    std::istringstream input("f @@ g;");
    Lexer lexer(input);
    auto tokens = lexer.tokenize();

    REQUIRE(tokens[0].type == TokenType::Identifier);
    REQUIRE(tokens[0].getValue<std::string>() == "f");
    REQUIRE(tokens[1].type == TokenType::AtAt);
    REQUIRE(tokens[2].type == TokenType::Identifier);
    REQUIRE(tokens[2].getValue<std::string>() == "g");
    REQUIRE(tokens[3].type == TokenType::Semicolon);
}

TEST_CASE("Operator Pipe", "[lexer][operators]")
{
    std::istringstream input("f | g;");
    Lexer lexer(input);
    auto tokens = lexer.tokenize();

    REQUIRE(tokens[0].type == TokenType::Identifier);
    REQUIRE(tokens[0].getValue<std::string>() == "f");
    REQUIRE(tokens[1].type == TokenType::Pipe);
    REQUIRE(tokens[2].type == TokenType::Identifier);
    REQUIRE(tokens[2].getValue<std::string>() == "g");
    REQUIRE(tokens[3].type == TokenType::Semicolon);
}

TEST_CASE("Logical AND operator", "[lexer][operators]")
{
    std::istringstream input("a && b;");
    Lexer lexer(input);
    auto tokens = lexer.tokenize();

    REQUIRE(tokens[0].type == TokenType::Identifier);
    REQUIRE(tokens[1].type == TokenType::And);
    REQUIRE(tokens[2].type == TokenType::Identifier);
    REQUIRE(tokens[3].type == TokenType::Semicolon);
}

TEST_CASE("Logical OR operator", "[lexer][operators]")
{
    std::istringstream input("a || b;");
    Lexer lexer(input);
    auto tokens = lexer.tokenize();

    REQUIRE(tokens[0].type == TokenType::Identifier);
    REQUIRE(tokens[1].type == TokenType::Or);
    REQUIRE(tokens[2].type == TokenType::Identifier);
    REQUIRE(tokens[3].type == TokenType::Semicolon);
}

TEST_CASE("Parentheses", "[lexer][symbols]")
{
    std::istringstream input("(a);");
    Lexer lexer(input);
    auto tokens = lexer.tokenize();

    REQUIRE(tokens[0].type == TokenType::LParen);
    REQUIRE(tokens[1].type == TokenType::Identifier);
    REQUIRE(tokens[2].type == TokenType::RParen);
    REQUIRE(tokens[3].type == TokenType::Semicolon);
}

TEST_CASE("Brackets", "[lexer][symbols]")
{
    std::istringstream input("[a];");
    Lexer lexer(input);
    auto tokens = lexer.tokenize();

    REQUIRE(tokens[0].type == TokenType::LBracket);
    REQUIRE(tokens[1].type == TokenType::Identifier);
    REQUIRE(tokens[2].type == TokenType::RBracket);
    REQUIRE(tokens[3].type == TokenType::Semicolon);
}

TEST_CASE("Comma", "[lexer][symbols]")
{
    std::istringstream input("fun f(var a, var b) [ return a; ]");
    Lexer lexer(input);
    auto tokens = lexer.tokenize();

    REQUIRE(tokens[0].type == TokenType::Fun);
    REQUIRE(tokens[1].type == TokenType::Identifier);
    REQUIRE(tokens[2].type == TokenType::LParen);
    REQUIRE(tokens[3].type == TokenType::Var);
    REQUIRE(tokens[4].type == TokenType::Identifier);
    REQUIRE(tokens[5].type == TokenType::Comma);
    REQUIRE(tokens[6].type == TokenType::Var);
    REQUIRE(tokens[7].type == TokenType::Identifier);
    REQUIRE(tokens[8].type == TokenType::RParen);
    REQUIRE(tokens[9].type == TokenType::LBracket);
    REQUIRE(tokens[10].type == TokenType::Return);
    REQUIRE(tokens[11].type == TokenType::Identifier);
    REQUIRE(tokens[12].type == TokenType::Semicolon);
    REQUIRE(tokens[13].type == TokenType::RBracket);
}

TEST_CASE("End of file token", "[lexer][EOF]")
{
    std::istringstream input("var x = 1;");
    Lexer lexer(input);
    auto tokens = lexer.tokenize();

    REQUIRE(!tokens.empty());
    REQUIRE(tokens.back().type == TokenType::EndOfFile);
}

TEST_CASE("Unknown character handling", "[lexer][error]")
{
    std::istringstream input("var x = 1 #;");
    Lexer lexer(input);
    auto tokens = lexer.tokenize();

    bool foundUnknown = false;
    for (const auto& token : tokens)
    {
        if (token.type == TokenType::Unknown && token.getValue<std::string>() == "#")
        {
            foundUnknown = true;
            break;
        }
    }

    REQUIRE(foundUnknown);
}

TEST_CASE("Comments handling", "[lexer][comments]")
{
    std::istringstream input("// This is a comment;");
    Lexer lexer(input);
    auto tokens = lexer.tokenize();

    REQUIRE(!tokens.empty());
}

TEST_CASE("Inline comment skipping", "[lexer][comments]")
{
    std::istringstream input("var x = 5; // initialize x\nx = x + 1;");
    Lexer lexer(input);
    auto tokens = lexer.tokenize();

    REQUIRE(tokens[0].type == TokenType::Var);
    REQUIRE(tokens[1].type == TokenType::Identifier);
    REQUIRE(tokens[2].type == TokenType::Assign);
    REQUIRE(tokens[3].type == TokenType::Number);
    REQUIRE(tokens[4].type == TokenType::Semicolon);
    REQUIRE(tokens[5].type == TokenType::Identifier);
    REQUIRE(tokens[6].type == TokenType::Assign);
    REQUIRE(tokens[7].type == TokenType::Identifier);
    REQUIRE(tokens[8].type == TokenType::Plus);
    REQUIRE(tokens[9].type == TokenType::Number);
    REQUIRE(tokens[10].type == TokenType::Semicolon);
}

TEST_CASE("Whitespace-only input", "[lexer][comments]")
{
    std::istringstream input("   \n\t  ");
    Lexer lexer(input);
    auto tokens = lexer.tokenize();

    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens[0].type == TokenType::EndOfFile);
}
