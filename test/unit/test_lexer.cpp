#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>
#include <sstream>
#include "lexer.hpp"
#include "interpreter_exception.hpp"

std::vector<Token> tokenize(Lexer* lexer)
{
    std::vector<Token> tokens;
    Token token = lexer->scanToken();

    while (token.type != TokenType::EndOfFile)
    {
        tokens.push_back(token);
        token = lexer->scanToken();
    }

    tokens.push_back(token);
    return tokens;
}

TEST_CASE("Identifier parsing", "[lexer][identifier]")
{
    std::istringstream input("alpha _temp1 x2 alpha_temp1;");
    Lexer lexer(input);

    auto tokens = tokenize(&lexer);

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

    auto tokens = tokenize(&lexer);

    REQUIRE(tokens[0].type == TokenType::Number);
    REQUIRE(tokens[0].getValue<int>() == 123);
    REQUIRE(tokens[1].type == TokenType::Identifier);
    REQUIRE(tokens[1].getValue<std::string>() == "xdd");
}

TEST_CASE("Too long identifier parsing", "[lexer][identifier]")
{
    std::string identifier = "";
    for (int i = 0; i < 50; ++i) identifier += 'a';
    std::istringstream input(identifier);
    Lexer lexer(input);

    try
    {
        auto tokens = tokenize(&lexer);
        FAIL("Expected InterpreterException not thrown");
    }
    catch (const InterpreterException& ex)
    {
        REQUIRE(ex.error.type == ErrorType::Lexical);
        REQUIRE(ex.error.message == "Identifier is too long");
        REQUIRE(ex.error.line == 1);
        REQUIRE(ex.error.column == 1);
    }
}

TEST_CASE("Identifier parsing with keyword", "[lexer][identifier]")
{
    std::istringstream input("var_a fun_a if_a");
    Lexer lexer(input);

    auto tokens = tokenize(&lexer);

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

    auto tokens = tokenize(&lexer);

    REQUIRE(tokens[0].type == TokenType::Number);
    REQUIRE(tokens[0].getValue<int>() == 123);
}

TEST_CASE("Number parsing out for range (int)", "[lexer][number]")
{
    std::istringstream input("2147483650;");
    Lexer lexer(input);

    auto tokens = tokenize(&lexer);

    REQUIRE(tokens[0].type == TokenType::Number);
    REQUIRE(tokens[0].getValue<int>() == 214748365);
}

TEST_CASE("Number parsing (float)", "[lexer][number]")
{
    std::istringstream input("45.67;");
    Lexer lexer(input);

    auto tokens = tokenize(&lexer);

    REQUIRE(tokens[0].type == TokenType::Number);
    REQUIRE(tokens[0].getValue<float>() == 45.67f);
}

TEST_CASE("Number parsing with doubled dot", "[lexer][number]")
{
    std::istringstream input("45.67.;");
    Lexer lexer(input);

    try
    {
        auto tokens = tokenize(&lexer);
        FAIL("Expected InterpreterException not thrown");
    }
    catch (const InterpreterException& ex)
    {
        REQUIRE(ex.error.type == ErrorType::Syntax);
        REQUIRE(ex.error.message == "Invalid float");
        REQUIRE(ex.error.line == 1);
        REQUIRE(ex.error.column == 1);
    }
}

TEST_CASE("String parsing", "[lexer][string]")
{
    std::istringstream input("\"Hi\"");
    Lexer lexer(input);

    auto tokens = tokenize(&lexer);

    REQUIRE(tokens[0].type == TokenType::StringLiteral);
    REQUIRE(std::get<std::string>(tokens[0].value.value()) == "Hi");
}

TEST_CASE("String parsing with numbers", "[lexer][string]")
{
    std::istringstream input("\"12345\"");
    Lexer lexer(input);

    auto tokens = tokenize(&lexer);

    REQUIRE(tokens[0].type == TokenType::StringLiteral);
    REQUIRE(std::get<std::string>(tokens[0].value.value()) == "12345");
}

TEST_CASE("String parsing no end quote", "[lexer][string]")
{
    std::istringstream input("\"12345");
    Lexer lexer(input);

    try
    {
        auto tokens = tokenize(&lexer);
        FAIL("Expected InterpreterException not thrown");
    }
    catch (const InterpreterException& ex)
    {
        REQUIRE(ex.error.type == ErrorType::Lexical);
        REQUIRE(ex.error.message == "Unterminated string literal");
        REQUIRE(ex.error.line == 1);
        REQUIRE(ex.error.column == 1);
    }
}

TEST_CASE("String with escape characters", "[lexer][string]")
{
    std::istringstream input(R"(var s = "\"quoted\" and \\slash";)");
    Lexer lexer(input);

    auto tokens = tokenize(&lexer);

    REQUIRE(tokens[3].type == TokenType::StringLiteral);
    REQUIRE(std::get<std::string>(tokens[3].value.value()) == "\"quoted\" and \\slash");
}

TEST_CASE("Variable keyword", "[lexer][keyword]")
{
    std::istringstream input("var");
    Lexer lexer(input);

    auto tokens = tokenize(&lexer);

    REQUIRE(tokens[0].type == TokenType::Var);
    REQUIRE(!tokens[0].value.has_value());
}

TEST_CASE("Keyword: const", "[lexer][keyword]")
{
    std::istringstream input("const");
    Lexer lexer(input);
    auto tokens = tokenize(&lexer);

    REQUIRE(tokens[0].type == TokenType::Const);
    REQUIRE(!tokens[0].value.has_value());
}

TEST_CASE("Keyword: fun", "[lexer][keyword]")
{
    std::istringstream input("fun");
    Lexer lexer(input);
    auto tokens = tokenize(&lexer);

    REQUIRE(tokens[0].type == TokenType::Fun);
    REQUIRE(!tokens[0].value.has_value());
}

TEST_CASE("Keyword: return", "[lexer][keyword]")
{
    std::istringstream input("return");
    Lexer lexer(input);
    auto tokens = tokenize(&lexer);

    REQUIRE(tokens[0].type == TokenType::Return);
    REQUIRE(!tokens[0].value.has_value());
}

TEST_CASE("Keyword: if", "[lexer][keyword]")
{
    std::istringstream input("if");
    Lexer lexer(input);
    auto tokens = tokenize(&lexer);

    REQUIRE(tokens[0].type == TokenType::If);
    REQUIRE(!tokens[0].value.has_value());
}

TEST_CASE("Keyword: else", "[lexer][keyword]")
{
    std::istringstream input("else");
    Lexer lexer(input);
    auto tokens = tokenize(&lexer);

    REQUIRE(tokens[0].type == TokenType::Else);
    REQUIRE(!tokens[0].value.has_value());
}

TEST_CASE("Keyword: while", "[lexer][keyword]")
{
    std::istringstream input("while");
    Lexer lexer(input);
    auto tokens = tokenize(&lexer);

    REQUIRE(tokens[0].type == TokenType::While);
    REQUIRE(!tokens[0].value.has_value());
}

TEST_CASE("Keyword: as", "[lexer][keyword]")
{
    std::istringstream input("as");
    Lexer lexer(input);
    auto tokens = tokenize(&lexer);

    REQUIRE(tokens[0].type == TokenType::As);
    REQUIRE(!tokens[0].value.has_value());
}

TEST_CASE("Keyword: print", "[lexer][keyword]")
{
    std::istringstream input("print");
    Lexer lexer(input);
    auto tokens = tokenize(&lexer);

    REQUIRE(tokens[0].type == TokenType::Print);
    REQUIRE(!tokens[0].value.has_value());
}

TEST_CASE("Operator: +", "[lexer][keyword]")
{
    std::istringstream input("+");
    Lexer lexer(input);
    auto tokens = tokenize(&lexer);

    REQUIRE(tokens[0].type == TokenType::Plus);
    REQUIRE(!tokens[0].value.has_value());
}

TEST_CASE("Operator: -", "[lexer][keyword]")
{
    std::istringstream input("-");
    Lexer lexer(input);
    auto tokens = tokenize(&lexer);

    REQUIRE(tokens[0].type == TokenType::Minus);
    REQUIRE(!tokens[0].value.has_value());
}

TEST_CASE("Operator: *", "[lexer][keyword]")
{
    std::istringstream input("*");
    Lexer lexer(input);
    auto tokens = tokenize(&lexer);

    REQUIRE(tokens[0].type == TokenType::Star);
    REQUIRE(!tokens[0].value.has_value());
}

TEST_CASE("Operator: /", "[lexer][keyword]")
{
    std::istringstream input("/");
    Lexer lexer(input);
    auto tokens = tokenize(&lexer);

    REQUIRE(tokens[0].type == TokenType::Slash);
    REQUIRE(!tokens[0].value.has_value());
}

TEST_CASE("Operator: ==", "[lexer][keyword]")
{
    std::istringstream input("==");
    Lexer lexer(input);
    auto tokens = tokenize(&lexer);

    REQUIRE(tokens[0].type == TokenType::Equal);
    REQUIRE(!tokens[0].value.has_value());
}

TEST_CASE("Operator: !=", "[lexer][operator]")
{
    std::istringstream input("!=");
    Lexer lexer(input);
    auto tokens = tokenize(&lexer);

    REQUIRE(tokens[0].type == TokenType::NotEqual);
    REQUIRE(!tokens[0].value.has_value());
}

TEST_CASE("Operator: >", "[lexer][operator]")
{
    std::istringstream input(">");
    Lexer lexer(input);
    auto tokens = tokenize(&lexer);

    REQUIRE(tokens[0].type == TokenType::Greater);
    REQUIRE(!tokens[0].value.has_value());
}

TEST_CASE("Operator: >=", "[lexer][operator]")
{
    std::istringstream input(">=");
    Lexer lexer(input);
    auto tokens = tokenize(&lexer);

    REQUIRE(tokens[0].type == TokenType::GreaterEqual);
    REQUIRE(!tokens[0].value.has_value());
}

TEST_CASE("Operator: <", "[lexer][operator]")
{
    std::istringstream input("<");
    Lexer lexer(input);
    auto tokens = tokenize(&lexer);

    REQUIRE(tokens[0].type == TokenType::Less);
    REQUIRE(!tokens[0].value.has_value());
}

TEST_CASE("Operator: <=", "[lexer][operator]")
{
    std::istringstream input("<=");
    Lexer lexer(input);
    auto tokens = tokenize(&lexer);

    REQUIRE(tokens[0].type == TokenType::LessEqual);
    REQUIRE(!tokens[0].value.has_value());
}

TEST_CASE("Operator: |", "[lexer][operator]")
{
    std::istringstream input("|");
    Lexer lexer(input);
    auto tokens = tokenize(&lexer);

    REQUIRE(tokens[0].type == TokenType::Pipe);
    REQUIRE(!tokens[0].value.has_value());
}

TEST_CASE("Operator: @@", "[lexer][operator]")
{
    std::istringstream input("@@");
    Lexer lexer(input);
    auto tokens = tokenize(&lexer);

    REQUIRE(tokens[0].type == TokenType::AtAt);
    REQUIRE(!tokens[0].value.has_value());
}

TEST_CASE("Operator: =", "[lexer][operator]")
{
    std::istringstream input("=");
    Lexer lexer(input);
    auto tokens = tokenize(&lexer);

    REQUIRE(tokens[0].type == TokenType::Assign);
    REQUIRE(!tokens[0].value.has_value());
}

TEST_CASE("Operator: &&", "[lexer][operator]")
{
    std::istringstream input("&&");
    Lexer lexer(input);
    auto tokens = tokenize(&lexer);

    REQUIRE(tokens[0].type == TokenType::And);
    REQUIRE(!tokens[0].value.has_value());
}

TEST_CASE("Operator: ||", "[lexer][operator]")
{
    std::istringstream input("||");
    Lexer lexer(input);
    auto tokens = tokenize(&lexer);

    REQUIRE(tokens[0].type == TokenType::Or);
    REQUIRE(!tokens[0].value.has_value());
}

TEST_CASE("Symbol: (", "[lexer][symbol]")
{
    std::istringstream input("(");
    Lexer lexer(input);
    auto tokens = tokenize(&lexer);

    REQUIRE(tokens[0].type == TokenType::LParen);
    REQUIRE(!tokens[0].value.has_value());
}

TEST_CASE("Symbol: )", "[lexer][symbol]")
{
    std::istringstream input(")");
    Lexer lexer(input);
    auto tokens = tokenize(&lexer);

    REQUIRE(tokens[0].type == TokenType::RParen);
    REQUIRE(!tokens[0].value.has_value());
}

TEST_CASE("Brackets", "[lexer][symbol]")
{
    SECTION("LBracket")
    {
        std::istringstream input("[");
        Lexer lexer(input);
        auto tokens = tokenize(&lexer);
        REQUIRE(tokens[0].type == TokenType::LBracket);
    }

    SECTION("RBracket")
    {
        std::istringstream input("]");
        Lexer lexer(input);
        auto tokens = tokenize(&lexer);
        REQUIRE(tokens[0].type == TokenType::RBracket);
    }
}

TEST_CASE("Symbol: ;", "[lexer][symbol]")
{
    std::istringstream input(";");
    Lexer lexer(input);
    auto tokens = tokenize(&lexer);

    REQUIRE(tokens[0].type == TokenType::Semicolon);
    REQUIRE(!tokens[0].value.has_value());
}

TEST_CASE("Symbol: ,", "[lexer][symbol]")
{
    std::istringstream input(",");
    Lexer lexer(input);
    auto tokens = tokenize(&lexer);

    REQUIRE(tokens[0].type == TokenType::Comma);
    REQUIRE(!tokens[0].value.has_value());
}

TEST_CASE("Symbol: EOF", "[lexer][symbol]")
{
    std::istringstream input("");
    Lexer lexer(input);
    auto tokens = tokenize(&lexer);

    REQUIRE(tokens[0].type == TokenType::EndOfFile);
    REQUIRE(!tokens[0].value.has_value());
    Token token = lexer.scanToken();
    REQUIRE(token.type == TokenType::EndOfFile);
    REQUIRE(!token.value.has_value());
}

TEST_CASE("Symbol: Unknown", "[lexer][symbol]")
{
    std::istringstream input(".");
    Lexer lexer(input);
    auto tokens = tokenize(&lexer);

    REQUIRE(tokens[0].type == TokenType::Unknown);
    REQUIRE(tokens[0].getValue<std::string>() == ".");
}

TEST_CASE("Simple variable declaration", "[lexer][keyword]")
{
    std::istringstream input("var x = 42;");
    Lexer lexer(input);

    auto tokens = tokenize(&lexer);

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

    auto tokens = tokenize(&lexer);

    REQUIRE(tokens[3].type == TokenType::StringLiteral);
    REQUIRE(tokens[3].getValue<std::string>() == "Hello\nWorld");
}

TEST_CASE("Type keywords recognized as TokenType::Type", "[lexer][type]")
{
    std::istringstream input("int float string");
    Lexer lexer(input);

    auto tokens = tokenize(&lexer);

    REQUIRE(tokens[0].type == TokenType::Type);
    REQUIRE(!tokens[0].value.has_value());
    REQUIRE(tokens[1].type == TokenType::Type);
    REQUIRE(!tokens[1].value.has_value());
    REQUIRE(tokens[2].type == TokenType::Type);
    REQUIRE(!tokens[2].value.has_value());
}

TEST_CASE("Constant variable declaration", "[lexer][keyword]")
{
    std::istringstream input("const var s = 12;");
    Lexer lexer(input);
    auto tokens = tokenize(&lexer);

    REQUIRE(tokens[0].type == TokenType::Const);
    REQUIRE(!tokens[0].value.has_value());
    REQUIRE(tokens[1].type == TokenType::Var);
    REQUIRE(tokens[2].type == TokenType::Identifier);
    REQUIRE(tokens[2].getValue<std::string>() == "s");
    REQUIRE(tokens[3].type == TokenType::Assign);
    REQUIRE(!tokens[3].value.has_value());
    REQUIRE(tokens[4].type == TokenType::Number);
    REQUIRE(tokens[4].getValue<int>() == 12);
    REQUIRE(tokens[5].type == TokenType::Semicolon);
    REQUIRE(!tokens[5].value.has_value());
}

TEST_CASE("Function declaration", "[lexer][keyword]")
{
    std::istringstream input("fun abc(var a) [return a;]");
    Lexer lexer(input);
    auto tokens = tokenize(&lexer);

    REQUIRE(tokens[0].type == TokenType::Fun);
    REQUIRE(!tokens[0].value.has_value());
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
    auto tokens = tokenize(&lexer);

    REQUIRE(tokens[0].type == TokenType::Fun);
    REQUIRE(!tokens[0].value.has_value());
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
    auto tokens = tokenize(&lexer);

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
    auto tokens = tokenize(&lexer);

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
    auto tokens = tokenize(&lexer);

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
    auto tokens = tokenize(&lexer);

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
    auto tokens = tokenize(&lexer);

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
    auto tokens = tokenize(&lexer);

    REQUIRE(tokens[0].type == TokenType::Identifier);
    REQUIRE(tokens[1].type == TokenType::Plus);
    REQUIRE(tokens[2].type == TokenType::Identifier);
    REQUIRE(tokens[3].type == TokenType::Semicolon);
}

TEST_CASE("Operator Minus", "[lexer][operators]")
{
    std::istringstream input("a - b;");
    Lexer lexer(input);
    auto tokens = tokenize(&lexer);

    REQUIRE(tokens[0].type == TokenType::Identifier);
    REQUIRE(tokens[1].type == TokenType::Minus);
    REQUIRE(tokens[2].type == TokenType::Identifier);
    REQUIRE(tokens[3].type == TokenType::Semicolon);
}

TEST_CASE("Operator Star", "[lexer][operators]")
{
    std::istringstream input("a * b;");
    Lexer lexer(input);
    auto tokens = tokenize(&lexer);

    REQUIRE(tokens[0].type == TokenType::Identifier);
    REQUIRE(tokens[1].type == TokenType::Star);
    REQUIRE(tokens[2].type == TokenType::Identifier);
    REQUIRE(tokens[3].type == TokenType::Semicolon);
}

TEST_CASE("Operator Slash", "[lexer][operators]")
{
    std::istringstream input("a / b;");
    Lexer lexer(input);
    auto tokens = tokenize(&lexer);

    REQUIRE(tokens[0].type == TokenType::Identifier);
    REQUIRE(tokens[1].type == TokenType::Slash);
    REQUIRE(tokens[2].type == TokenType::Identifier);
    REQUIRE(tokens[3].type == TokenType::Semicolon);
}

TEST_CASE("Operator Assign", "[lexer][operators]")
{
    std::istringstream input("a = b;");
    Lexer lexer(input);
    auto tokens = tokenize(&lexer);

    REQUIRE(tokens[0].type == TokenType::Identifier);
    REQUIRE(tokens[1].type == TokenType::Assign);
    REQUIRE(tokens[2].type == TokenType::Identifier);
    REQUIRE(tokens[3].type == TokenType::Semicolon);
}

TEST_CASE("Operator EqualEqual", "[lexer][operators]")
{
    std::istringstream input("a == b;");
    Lexer lexer(input);
    auto tokens = tokenize(&lexer);

    REQUIRE(tokens[0].type == TokenType::Identifier);
    REQUIRE(tokens[1].type == TokenType::Equal);
    REQUIRE(tokens[2].type == TokenType::Identifier);
    REQUIRE(tokens[3].type == TokenType::Semicolon);
}

TEST_CASE("Operator NotEqual", "[lexer][operators]")
{
    std::istringstream input("a != b;");
    Lexer lexer(input);
    auto tokens = tokenize(&lexer);

    REQUIRE(tokens[0].type == TokenType::Identifier);
    REQUIRE(tokens[1].type == TokenType::NotEqual);
    REQUIRE(tokens[2].type == TokenType::Identifier);
    REQUIRE(tokens[3].type == TokenType::Semicolon);
}

TEST_CASE("Operator Greater", "[lexer][operators]")
{
    std::istringstream input("a > b;");
    Lexer lexer(input);
    auto tokens = tokenize(&lexer);

    REQUIRE(tokens[0].type == TokenType::Identifier);
    REQUIRE(tokens[1].type == TokenType::Greater);
    REQUIRE(tokens[2].type == TokenType::Identifier);
    REQUIRE(tokens[3].type == TokenType::Semicolon);
}

TEST_CASE("Operator GreaterEqual", "[lexer][operators]")
{
    std::istringstream input("a >= b;");
    Lexer lexer(input);
    auto tokens = tokenize(&lexer);

    REQUIRE(tokens[0].type == TokenType::Identifier);
    REQUIRE(tokens[1].type == TokenType::GreaterEqual);
    REQUIRE(tokens[2].type == TokenType::Identifier);
    REQUIRE(tokens[3].type == TokenType::Semicolon);
}

TEST_CASE("Operator Less", "[lexer][operators]")
{
    std::istringstream input("a < b;");
    Lexer lexer(input);
    auto tokens = tokenize(&lexer);

    REQUIRE(tokens[0].type == TokenType::Identifier);
    REQUIRE(tokens[1].type == TokenType::Less);
    REQUIRE(tokens[2].type == TokenType::Identifier);
    REQUIRE(tokens[3].type == TokenType::Semicolon);
}

TEST_CASE("Operator LessEqual", "[lexer][operators]")
{
    std::istringstream input("a <= b;");
    Lexer lexer(input);
    auto tokens = tokenize(&lexer);

    REQUIRE(tokens[0].type == TokenType::Identifier);
    REQUIRE(tokens[1].type == TokenType::LessEqual);
    REQUIRE(tokens[2].type == TokenType::Identifier);
    REQUIRE(tokens[3].type == TokenType::Semicolon);
}

TEST_CASE("Operator AtAt", "[lexer][operators]")
{
    std::istringstream input("f @@ g;");
    Lexer lexer(input);
    auto tokens = tokenize(&lexer);

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
    auto tokens = tokenize(&lexer);

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
    auto tokens = tokenize(&lexer);

    REQUIRE(tokens[0].type == TokenType::Identifier);
    REQUIRE(tokens[1].type == TokenType::And);
    REQUIRE(tokens[2].type == TokenType::Identifier);
    REQUIRE(tokens[3].type == TokenType::Semicolon);
}

TEST_CASE("Logical OR operator", "[lexer][operators]")
{
    std::istringstream input("a || b;");

    Lexer lexer(input);
    auto tokens = tokenize(&lexer);

    REQUIRE(tokens[0].type == TokenType::Identifier);
    REQUIRE(tokens[1].type == TokenType::Or);
    REQUIRE(tokens[2].type == TokenType::Identifier);
    REQUIRE(tokens[3].type == TokenType::Semicolon);
}

TEST_CASE("Parentheses", "[lexer][symbols]")
{
    std::istringstream input("(a);");
    Lexer lexer(input);
    auto tokens = tokenize(&lexer);

    REQUIRE(tokens[0].type == TokenType::LParen);
    REQUIRE(tokens[1].type == TokenType::Identifier);
    REQUIRE(tokens[2].type == TokenType::RParen);
    REQUIRE(tokens[3].type == TokenType::Semicolon);
}

TEST_CASE("Brackets", "[lexer][symbols]")
{
    std::istringstream input("[a];");
    Lexer lexer(input);
    auto tokens = tokenize(&lexer);

    REQUIRE(tokens[0].type == TokenType::LBracket);
    REQUIRE(tokens[1].type == TokenType::Identifier);
    REQUIRE(tokens[2].type == TokenType::RBracket);
    REQUIRE(tokens[3].type == TokenType::Semicolon);
}

TEST_CASE("Comma", "[lexer][symbols]")
{
    std::istringstream input("fun f(var a, var b) [ return a; ]");
    Lexer lexer(input);
    auto tokens = tokenize(&lexer);

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
    auto tokens = tokenize(&lexer);

    REQUIRE(!tokens.empty());
    REQUIRE(tokens.back().type == TokenType::EndOfFile);
}

TEST_CASE("Unknown character handling", "[lexer][error]")
{
    std::istringstream input("var x = 1 #;");
    Lexer lexer(input);
    auto tokens = tokenize(&lexer);

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
    auto tokens = tokenize(&lexer);

    REQUIRE(!tokens.empty());
}

TEST_CASE("Inline comment skipping", "[lexer][comments]")
{
    std::istringstream input("var x = 5; // initialize x\nx = x + 1;");
    Lexer lexer(input);
    auto tokens = tokenize(&lexer);

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
    auto tokens = tokenize(&lexer);

    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens[0].type == TokenType::EndOfFile);
}

TEST_CASE("Lexer handles CRLF line endings", "[lexer][crlf]")
{
    std::istringstream input("var a = 5;\r\nvar b = 10;\r\n");
    Lexer lexer(input);
    auto tokens = tokenize(&lexer);

    REQUIRE(tokens.size() > 0);

    bool foundB = false;
    for (const auto& token : tokens)
    {
        if (token.type == TokenType::Identifier && token.getValue<std::string>() == "b")
        {
            foundB = true;
            REQUIRE(token.line == 2);
            REQUIRE(token.column == 5);
        }
    }

    REQUIRE(foundB);
}
