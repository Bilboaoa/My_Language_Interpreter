#include <memory>

#include "catch2/catch_all.hpp"

#include "asTree.hpp"
#include "parser.hpp"
#include "error.hpp"
#include "parserVisitor.hpp"

class ParserTester
{
   public:
    std::istringstream stream;
    Lexer lexer;
    Parser parser;

    ParserTester(const std::string& input) : stream(input), lexer(stream), parser(lexer) {}
};

TEST_CASE("Test Minimal Declaration", "[parser][declaration]")
{
    ParserTester parserTester("var x;");
    auto program = parserTester.parser.parseProgram();
    ParserVisitor visitor;

    REQUIRE(program != nullptr);
    REQUIRE(program->declarations.size() == 1);

    std::string expected = "Var x;\n";
    program->accept(visitor, 0);
    std::string programString = visitor.getParsedString();
    REQUIRE(programString == expected);
}

TEST_CASE("Test Minimal const Declaration", "[parser][declaration]")
{
    ParserTester parserTester("const x;");
    auto program = parserTester.parser.parseProgram();
    ParserVisitor visitor;

    REQUIRE(program != nullptr);
    REQUIRE(program->declarations.size() == 1);

    std::string expected = "Const x;\n";
    program->accept(visitor, 0);
    std::string programString = visitor.getParsedString();
    REQUIRE(programString == expected);
}

TEST_CASE("Test multiple Minimal const Declaration", "[parser][declaration]")
{
    ParserTester parserTester("const x; var z; const y;");
    auto program = parserTester.parser.parseProgram();
    ParserVisitor visitor;

    REQUIRE(program != nullptr);
    REQUIRE(program->declarations.size() == 3);

    std::string expected = "Const x;\nVar z;\nConst y;\n";
    program->accept(visitor, 0);
    std::string programString = visitor.getParsedString();
    REQUIRE(programString == expected);
}

TEST_CASE("Test Minimal function Declaration", "[parser][function]")
{
    ParserTester parserTester("fun a() []");
    auto program = parserTester.parser.parseProgram();
    ParserVisitor visitor;

    REQUIRE(program != nullptr);
    REQUIRE(program->declarations.size() == 1);

    std::string expected = "Fun a()\n [\n ]\n";
    program->accept(visitor, 0);
    std::string programString = visitor.getParsedString();
    REQUIRE(programString == expected);
}

TEST_CASE("Test Minimal function Declaration with body", "[parser][function][assign]")
{
    ParserTester parserTester("fun a() [var a=2;]");
    auto program = parserTester.parser.parseProgram();
    ParserVisitor visitor;

    REQUIRE(program != nullptr);
    REQUIRE(program->declarations.size() == 1);

    std::string expected = "Fun a()\n [\n  Var a = 2;\n ]\n";
    program->accept(visitor, 0);
    std::string programString = visitor.getParsedString();
    REQUIRE(programString == expected);
}

TEST_CASE("Test Minimal function Declaration with body and one argument",
          "[parser][function][assign]")
{
    ParserTester parserTester("fun a(var a) [a=2;]");
    auto program = parserTester.parser.parseProgram();
    ParserVisitor visitor;

    REQUIRE(program != nullptr);
    REQUIRE(program->declarations.size() == 1);

    std::string expected = "Fun a(Var a)\n [\n  a = 2;\n ]\n";
    program->accept(visitor, 0);
    std::string programString = visitor.getParsedString();
    REQUIRE(programString == expected);
}

TEST_CASE("Test Minimal function Declaration with body and many arguments",
          "[parser][function][assign]")
{
    ParserTester parserTester("fun a(var a, const b, var c) [a=2;]");
    auto program = parserTester.parser.parseProgram();
    ParserVisitor visitor;

    REQUIRE(program != nullptr);
    REQUIRE(program->declarations.size() == 1);

    std::string expected = "Fun a(Var a, Const b, Var c)\n [\n  a = 2;\n ]\n";
    program->accept(visitor, 0);
    std::string programString = visitor.getParsedString();
    REQUIRE(programString == expected);
}

TEST_CASE("Test Minimal function Declaration with body and wrong arguments",
          "[parser][function][assign]")
{
    ParserTester parserTester("fun a(var a, b, var c) [a=2;]");
    try
    {
        auto program = parserTester.parser.parseProgram();
    }
    catch (const InterpreterException& ex)
    {
        REQUIRE(ex.error.type == ErrorType::Semantic);
        REQUIRE(ex.error.message == "Expected 'const' or 'var'");
        REQUIRE(ex.error.startPosition == Position(1, 14));
    }
}

TEST_CASE("Test Minimal function Declaration with body and comma after an argument",
          "[parser][function][assign]")
{
    ParserTester parserTester("fun a(var a, const b,) [a=2;]");
    try
    {
        auto program = parserTester.parser.parseProgram();
    }
    catch (const InterpreterException& ex)
    {
        REQUIRE(ex.error.type == ErrorType::Semantic);
        REQUIRE(ex.error.message == "Expected 'const' or 'var'");
        REQUIRE(ex.error.startPosition == Position(1, 22));
    }
}

TEST_CASE("Test Minimal function Declaration with no identifier (not lambda)", "[parser][function]")
{
    ParserTester parserTester("fun (var b, var c) [b=2;]");
    try
    {
        auto program = parserTester.parser.parseProgram();
    }
    catch (const InterpreterException& ex)
    {
        REQUIRE(ex.error.type == ErrorType::Semantic);
        REQUIRE(ex.error.message == "Expected function's name");
        REQUIRE(ex.error.startPosition == Position(1, 5));
    }
}

TEST_CASE("Test Minimal function Declaration with no body (no brackets)", "[parser][function]")
{
    ParserTester parserTester("fun a(var b, var c)");
    try
    {
        auto program = parserTester.parser.parseProgram();
    }
    catch (const InterpreterException& ex)
    {
        REQUIRE(ex.error.type == ErrorType::Semantic);
        REQUIRE(ex.error.message == "Expected '['");
        REQUIRE(ex.error.startPosition == Position(1, 20));
    }
}

TEST_CASE("Test assign with float", "[parser][assign]")
{
    ParserTester parserTester("var a=2.1;");
    auto program = parserTester.parser.parseProgram();
    ParserVisitor visitor;

    REQUIRE(program != nullptr);
    REQUIRE(program->declarations.size() == 1);

    std::string expected = "Var a = 2.100000;\n";
    program->accept(visitor, 0);
    std::string programString = visitor.getParsedString();
    REQUIRE(programString == expected);
}

TEST_CASE("Test assign with string literal", "[parser][assign][string]")
{
    ParserTester parserTester("var a=\"abc\";");
    auto program = parserTester.parser.parseProgram();
    ParserVisitor visitor;

    REQUIRE(program != nullptr);
    REQUIRE(program->declarations.size() == 1);

    std::string expected = "Var a = \"abc\";\n";
    program->accept(visitor, 0);
    std::string programString = visitor.getParsedString();
    REQUIRE(programString == expected);
}

TEST_CASE("Test assign with function literal (no args)", "[parser][assign][function]")
{
    ParserTester parserTester("var a=fun()[return 1;];");
    auto program = parserTester.parser.parseProgram();
    ParserVisitor visitor;

    REQUIRE(program != nullptr);
    REQUIRE(program->declarations.size() == 1);

    std::string expected = R"(Var a = Fun()
  [
   return 1;
  ];
)";
    program->accept(visitor, 0);
    std::string programString = visitor.getParsedString();
    REQUIRE(programString == expected);
}

TEST_CASE("Test assign with nothing after =", "[parser][assign][error]")
{
    ParserTester parserTester("var a=");
    try
    {
        auto program = parserTester.parser.parseProgram();
    }
    catch (const InterpreterException& ex)
    {
        REQUIRE(ex.error.type == ErrorType::Semantic);
        REQUIRE(ex.error.message == "Expected an expression after assign");
        REQUIRE(ex.error.startPosition == Position(1, 7));
    }
}

TEST_CASE("Test assign with function literal (with args)", "[parser][assign][function]")
{
    ParserTester parserTester("var a=fun(var a, const b)[return a;];");
    auto program = parserTester.parser.parseProgram();
    ParserVisitor visitor;

    REQUIRE(program != nullptr);
    REQUIRE(program->declarations.size() == 1);

    std::string expected = R"(Var a = Fun(Var a, Const b)
  [
   return a;
  ];
)";
    program->accept(visitor, 0);
    std::string programString = visitor.getParsedString();
    REQUIRE(programString == expected);
}

TEST_CASE("Test assign after declaration", "[parser][function][assign]")
{
    ParserTester parserTester("fun a() [var a; a=2;]");
    auto program = parserTester.parser.parseProgram();
    ParserVisitor visitor;

    REQUIRE(program != nullptr);
    REQUIRE(program->declarations.size() == 1);

    std::string expected = "Fun a()\n [\n  Var a;\n  a = 2;\n ]\n";
    program->accept(visitor, 0);
    std::string programString = visitor.getParsedString();
    REQUIRE(programString == expected);
}

TEST_CASE("Test function declaration with return", "[parser][function][return]")
{
    ParserTester parserTester("fun a() [var a=2;return a;]");
    auto program = parserTester.parser.parseProgram();
    ParserVisitor visitor;

    REQUIRE(program != nullptr);
    REQUIRE(program->declarations.size() == 1);

    std::string expected = "Fun a()\n [\n  Var a = 2;\n  return a;\n ]\n";
    program->accept(visitor, 0);
    std::string programString = visitor.getParsedString();
    REQUIRE(programString == expected);
}

TEST_CASE("Test function declaration with empty return", "[parser][function][assign]")
{
    ParserTester parserTester("fun a() [var b = 2; return;]");
    auto program = parserTester.parser.parseProgram();
    ParserVisitor visitor;

    REQUIRE(program != nullptr);
    REQUIRE(program->declarations.size() == 1);

    std::string expected = "Fun a()\n [\n  Var b = 2;\n  return;\n ]\n";
    program->accept(visitor, 0);
    std::string programString = visitor.getParsedString();
    REQUIRE(programString == expected);
}

TEST_CASE("Test missing function body", "[parser][error]")
{
    ParserTester parserTester("fun a()");
    REQUIRE_THROWS_WITH(parserTester.parser.parseProgram(), "SemanticError at 1:8 → Expected '['");
}

TEST_CASE("Test function call within return", "[parser][function][return][call]")
{
    ParserTester parserTester("fun a() [var c=2;return c;] \nfun b()[return a();]");
    auto program = parserTester.parser.parseProgram();
    ParserVisitor visitor;

    REQUIRE(program != nullptr);
    REQUIRE(program->declarations.size() == 2);

    std::string expected =
        "Fun a()\n [\n  Var c = 2;\n  return c;\n ]\nFun b()\n [\n  return a();\n ]\n";
    program->accept(visitor, 0);
    std::string programString = visitor.getParsedString();
    REQUIRE(programString == expected);
}

TEST_CASE("Test complex expression within return", "[parser][function][return][call]")
{
    ParserTester parserTester(
        "fun a() [var c=2;return c;] \nfun b()[var q = 1;\nreturn (a() * q) / 10;]");
    auto program = parserTester.parser.parseProgram();
    ParserVisitor visitor;

    REQUIRE(program != nullptr);
    REQUIRE(program->declarations.size() == 2);

    std::string expected =
        "Fun a()\n [\n  Var c = 2;\n  return c;\n ]\nFun b()\n [\n  Var q = 1;\n  return a() Star "
        "q Slash 10;\n ]\n";
    program->accept(visitor, 0);
    std::string programString = visitor.getParsedString();
    REQUIRE(programString == expected);
}

TEST_CASE("Test function call within assign", "[parser][function][assign][call]")
{
    ParserTester parserTester("fun a(var s) [var c=2;return c;] \nvar b=a(1) + 1;");
    auto program = parserTester.parser.parseProgram();
    ParserVisitor visitor;

    REQUIRE(program != nullptr);
    REQUIRE(program->declarations.size() == 2);

    std::string expected =
        "Fun a(Var s)\n [\n  Var c = 2;\n  return c;\n ]\nVar b = a(1) Plus 1;\n";
    program->accept(visitor, 0);
    std::string programString = visitor.getParsedString();
    REQUIRE(programString == expected);
}

TEST_CASE("Test function call with complex arguments", "[parser][function][call]")
{
    ParserTester parserTester("fun a() [var x = b(1+2, c(3), 4);]");
    auto program = parserTester.parser.parseProgram();
    ParserVisitor visitor;

    std::string expected = R"(Fun a()
 [
  Var x = b(1 Plus 2, c(3), 4);
 ]
)";
    program->accept(visitor, 0);
    std::string programString = visitor.getParsedString();
    REQUIRE(programString == expected);
}

TEST_CASE("Test function call as statement", "[parser][function][call]")
{
    ParserTester parserTester("fun a() [b(1);]");
    auto program = parserTester.parser.parseProgram();
    ParserVisitor visitor;
    std::string expected = R"(Fun a()
 [
  b(1);
 ]
)";
    program->accept(visitor, 0);
    std::string programString = visitor.getParsedString();
    REQUIRE(programString == expected);
}

TEST_CASE("Test call with 2 parethesis", "[parser][function][call]")
{
    ParserTester parserTester("fun a() [return 1;]\n fun b() [return a;]\n fun c() [var d=b()();]");
    auto program = parserTester.parser.parseProgram();
    ParserVisitor visitor;
    REQUIRE(program != nullptr);
    REQUIRE(program->declarations.size() == 3);

    std::string expected = R"(Fun a()
 [
  return 1;
 ]
Fun b()
 [
  return a;
 ]
Fun c()
 [
  Var d = b()();
 ]
)";
    program->accept(visitor, 0);
    std::string programString = visitor.getParsedString();
    REQUIRE(programString == expected);
}

TEST_CASE("Test assign function to a variable", "[parser][function][assign]")
{
    ParserTester parserTester("fun a(var s) [var c=2;return c;] \nvar b=a;");
    auto program = parserTester.parser.parseProgram();
    ParserVisitor visitor;
    REQUIRE(program != nullptr);
    REQUIRE(program->declarations.size() == 2);

    std::string expected = "Fun a(Var s)\n [\n  Var c = 2;\n  return c;\n ]\nVar b = a;\n";
    program->accept(visitor, 0);
    std::string programString = visitor.getParsedString();
    REQUIRE(programString == expected);
}

TEST_CASE("Test function declaration with add", "[parser][function][plus]")
{
    ParserTester parserTester("fun a() [var a=2+2;]");
    auto program = parserTester.parser.parseProgram();
    ParserVisitor visitor;
    REQUIRE(program != nullptr);
    REQUIRE(program->declarations.size() == 1);

    std::string expected = "Fun a()\n [\n  Var a = 2 Plus 2;\n ]\n";
    program->accept(visitor, 0);
    std::string programString = visitor.getParsedString();
    REQUIRE(programString == expected);
}

TEST_CASE("Test function declaration with sub", "[parser][function][minus]")
{
    ParserTester parserTester("fun a() [var a=2-2;]");
    auto program = parserTester.parser.parseProgram();
    ParserVisitor visitor;
    REQUIRE(program != nullptr);
    REQUIRE(program->declarations.size() == 1);

    std::string expected = "Fun a()\n [\n  Var a = 2 Minus 2;\n ]\n";
    program->accept(visitor, 0);
    std::string programString = visitor.getParsedString();
    REQUIRE(programString == expected);
}

TEST_CASE("Test function declaration with mult", "[parser][function][star]")
{
    ParserTester parserTester("fun a() [var a=2*2;]");
    auto program = parserTester.parser.parseProgram();
    ParserVisitor visitor;
    REQUIRE(program != nullptr);
    REQUIRE(program->declarations.size() == 1);

    std::string expected = "Fun a()\n [\n  Var a = 2 Star 2;\n ]\n";
    program->accept(visitor, 0);
    std::string programString = visitor.getParsedString();
    REQUIRE(programString == expected);
}

TEST_CASE("Test function declaration with division", "[parser][function][slash]")
{
    ParserTester parserTester("fun a() [var a=2/2;]");
    auto program = parserTester.parser.parseProgram();
    ParserVisitor visitor;

    REQUIRE(program != nullptr);
    REQUIRE(program->declarations.size() == 1);

    std::string expected = "Fun a()\n [\n  Var a = 2 Slash 2;\n ]\n";
    program->accept(visitor, 0);
    std::string programString = visitor.getParsedString();
    REQUIRE(programString == expected);
}

TEST_CASE("Test function declaration with @@", "[parser][function][AtAt]")
{
    ParserTester parserTester("fun a() [var a=2@@1;]");
    auto program = parserTester.parser.parseProgram();
    ParserVisitor visitor;

    REQUIRE(program != nullptr);
    REQUIRE(program->declarations.size() == 1);

    std::string expected = "Fun a()\n [\n  Var a = 2 AtAt 1;\n ]\n";
    program->accept(visitor, 0);
    std::string programString = visitor.getParsedString();
    REQUIRE(programString == expected);
}

TEST_CASE("Test function declaration with |", "[parser][function][Pipe]")
{
    ParserTester parserTester("fun a() [var a=2|1;]");
    auto program = parserTester.parser.parseProgram();
    ParserVisitor visitor;
    REQUIRE(program != nullptr);
    REQUIRE(program->declarations.size() == 1);

    std::string expected = "Fun a()\n [\n  Var a = 2 Pipe 1;\n ]\n";
    program->accept(visitor, 0);
    std::string programString = visitor.getParsedString();
    REQUIRE(programString == expected);
}

TEST_CASE("Test expression with 2 functions", "[parser][function][Pipe]")
{
    ParserTester parserTester(
        "fun a() [return 1;]\n fun b() [return 2;]\n fun c() [var d=a()|b();]");
    auto program = parserTester.parser.parseProgram();
    ParserVisitor visitor;

    REQUIRE(program != nullptr);
    REQUIRE(program->declarations.size() == 3);

    std::string expected = R"(Fun a()
 [
  return 1;
 ]
Fun b()
 [
  return 2;
 ]
Fun c()
 [
  Var d = a() Pipe b();
 ]
)";
    program->accept(visitor, 0);
    std::string programString = visitor.getParsedString();
    REQUIRE(programString == expected);
}

TEST_CASE("Test declaration with assigned string concatination", "[parser][Plus]")
{
    ParserTester parserTester("var a=\"abc\"+\"de\";");
    auto program = parserTester.parser.parseProgram();
    ParserVisitor visitor;
    REQUIRE(program != nullptr);
    REQUIRE(program->declarations.size() == 1);

    std::string expected = "Var a = \"abc\" Plus \"de\";\n";
    program->accept(visitor, 0);
    std::string programString = visitor.getParsedString();
    REQUIRE(programString == expected);
}

TEST_CASE("Test nested arithmetic expressions", "[parser][arithmetic]")
{
    ParserTester parserTester("fun a() [var x = 1 + 2 * 3 / 4 - 5;]");
    auto program = parserTester.parser.parseProgram();
    ParserVisitor visitor;

    REQUIRE(program != nullptr);

    std::string expected = R"(Fun a()
 [
  Var x = 1 Plus 2 Star 3 Slash 4 Minus 5;
 ]
)";
    program->accept(visitor, 0);
    std::string programString = visitor.getParsedString();
    REQUIRE(programString == expected);
}

TEST_CASE("Test nested arithmetic expressions with parentheses", "[parser][arithmetic]")
{
    ParserTester parserTester("fun a() [var x = 1 + (2 * 3) / 4 - 5;]");
    auto program = parserTester.parser.parseProgram();
    ParserVisitor visitor;
    REQUIRE(program != nullptr);

    std::string expected = R"(Fun a()
 [
  Var x = 1 Plus 2 Star 3 Slash 4 Minus 5;
 ]
)";
    program->accept(visitor, 0);
    std::string programString = visitor.getParsedString();
    REQUIRE(programString == expected);
}

TEST_CASE("Test complex expression with 2 parentheses", "[parser][expression]")
{
    ParserTester parserTester("fun a() [var x = (1 + 2) * (3 - 4);]");
    auto program = parserTester.parser.parseProgram();
    ParserVisitor visitor;

    std::string expected = R"(Fun a()
 [
  Var x = 1 Plus 2 Star 3 Minus 4;
 ]
)";
    program->accept(visitor, 0);
    std::string programString = visitor.getParsedString();
    REQUIRE(programString == expected);
}

TEST_CASE("Test complex expression as statement", "[parser][expression]")
{
    ParserTester parserTester("fun a() [(1 + 2) * (3 - 4);]");
    auto program = parserTester.parser.parseProgram();
    ParserVisitor visitor;
    std::string expected = R"(Fun a()
 [
  1 Plus 2 Star 3 Minus 4;
 ]
)";
    program->accept(visitor, 0);
    std::string programString = visitor.getParsedString();
    REQUIRE(programString == expected);
}

TEST_CASE("Test nested arithmetic expressions with parentheses at start", "[parser][arithmetic]")
{
    ParserTester parserTester("fun a() [var x = (1 + 2) * 3 / 4 - 5;]");
    auto program = parserTester.parser.parseProgram();
    ParserVisitor visitor;
    REQUIRE(program != nullptr);

    std::string expected = R"(Fun a()
 [
  Var x = 1 Plus 2 Star 3 Slash 4 Minus 5;
 ]
)";
    program->accept(visitor, 0);
    std::string programString = visitor.getParsedString();
    REQUIRE(programString == expected);
}

TEST_CASE("Test function concatination with function literal (with args)",
          "[parser][Pipe][function]")
{
    ParserTester parserTester("var h = fun(var a, const b)[return a;] | fun(var b)[return b;];");
    auto program = parserTester.parser.parseProgram();
    ParserVisitor visitor;
    REQUIRE(program != nullptr);
    REQUIRE(program->declarations.size() == 1);

    std::string expected = R"(Var h = Fun(Var a, Const b)
   [
    return a;
   ] Pipe Fun(Var b)
   [
    return b;
   ];
)";
    program->accept(visitor, 0);
    std::string programString = visitor.getParsedString();
    REQUIRE(programString == expected);
}

TEST_CASE("Test type cast with int", "[parser][function][type][cast]")
{
    ParserTester parserTester("fun a() [var a=2 as int;]");
    auto program = parserTester.parser.parseProgram();
    ParserVisitor visitor;
    REQUIRE(program != nullptr);
    REQUIRE(program->declarations.size() == 1);

    std::string expected = "Fun a()\n [\n  Var a = 2 As int;\n ]\n";
    program->accept(visitor, 0);
    std::string programString = visitor.getParsedString();
    REQUIRE(programString == expected);
}

TEST_CASE("Test type cast with float", "[parser][function][type][cast]")
{
    ParserTester parserTester("fun a() [var a=2 as float;]");
    auto program = parserTester.parser.parseProgram();
    ParserVisitor visitor;
    REQUIRE(program != nullptr);
    REQUIRE(program->declarations.size() == 1);

    std::string expected = "Fun a()\n [\n  Var a = 2 As float;\n ]\n";
    program->accept(visitor, 0);
    std::string programString = visitor.getParsedString();
    REQUIRE(programString == expected);
}

TEST_CASE("Test type cast with string", "[parser][function][type][cast]")
{
    ParserTester parserTester("fun a() [var a=2 as string;]");
    auto program = parserTester.parser.parseProgram();
    ParserVisitor visitor;
    REQUIRE(program != nullptr);
    REQUIRE(program->declarations.size() == 1);

    std::string expected = "Fun a()\n [\n  Var a = 2 As string;\n ]\n";
    program->accept(visitor, 0);
    std::string programString = visitor.getParsedString();
    REQUIRE(programString == expected);
}

TEST_CASE("Test type cast with multiple types", "[parser][type][cast]")
{
    ParserTester parserTester("var a=2 as float as string;");
    auto program = parserTester.parser.parseProgram();
    ParserVisitor visitor;
    REQUIRE(program != nullptr);
    REQUIRE(program->declarations.size() == 1);

    std::string expected = "Var a = 2 As float As string;\n";
    program->accept(visitor, 0);
    std::string programString = visitor.getParsedString();
    REQUIRE(programString == expected);
}

TEST_CASE("Test basic If", "[parser][If][Less]")
{
    ParserTester parserTester("fun a() [if(1<2)[return 1;]return 2;]");
    auto program = parserTester.parser.parseProgram();
    ParserVisitor visitor;
    REQUIRE(program != nullptr);
    REQUIRE(program->declarations.size() == 1);

    std::string expected = R"(Fun a()
 [
  if (1 Less 2)
   [
    return 1;
   ]
  return 2;
 ]
)";
    program->accept(visitor, 0);
    std::string programString = visitor.getParsedString();
    REQUIRE(programString == expected);
}

TEST_CASE("Test basic If greater", "[parser][If][Greater]")
{
    ParserTester parserTester("fun a() [if(1>2)[return 1;]return 2;]");
    auto program = parserTester.parser.parseProgram();
    ParserVisitor visitor;
    REQUIRE(program != nullptr);
    REQUIRE(program->declarations.size() == 1);

    std::string expected = R"(Fun a()
 [
  if (1 Greater 2)
   [
    return 1;
   ]
  return 2;
 ]
)";
    program->accept(visitor, 0);
    std::string programString = visitor.getParsedString();
    REQUIRE(programString == expected);
}

TEST_CASE("Test basic If LessEqual", "[parser][If][LessEqual]")
{
    ParserTester parserTester("fun a() [if(1<=2)[return 1;]return 2;]");
    auto program = parserTester.parser.parseProgram();
    ParserVisitor visitor;
    REQUIRE(program != nullptr);
    REQUIRE(program->declarations.size() == 1);

    std::string expected = R"(Fun a()
 [
  if (1 LessEqual 2)
   [
    return 1;
   ]
  return 2;
 ]
)";
    program->accept(visitor, 0);
    std::string programString = visitor.getParsedString();
    REQUIRE(programString == expected);
}

TEST_CASE("Test basic If GreaterEqual", "[parser][If][GreaterEqual]")
{
    ParserTester parserTester("fun a() [if(1>=2)[return 1;]return 2;]");
    auto program = parserTester.parser.parseProgram();
    ParserVisitor visitor;
    REQUIRE(program != nullptr);
    REQUIRE(program->declarations.size() == 1);

    std::string expected = R"(Fun a()
 [
  if (1 GreaterEqual 2)
   [
    return 1;
   ]
  return 2;
 ]
)";
    program->accept(visitor, 0);
    std::string programString = visitor.getParsedString();
    REQUIRE(programString == expected);
}

TEST_CASE("Test basic If Equal", "[parser][If][Equal]")
{
    ParserTester parserTester("fun a() [if(1==2)[return 1;]return 2;]");
    auto program = parserTester.parser.parseProgram();
    ParserVisitor visitor;
    REQUIRE(program != nullptr);
    REQUIRE(program->declarations.size() == 1);

    std::string expected = R"(Fun a()
 [
  if (1 Equal 2)
   [
    return 1;
   ]
  return 2;
 ]
)";
    program->accept(visitor, 0);
    std::string programString = visitor.getParsedString();
    REQUIRE(programString == expected);
}

TEST_CASE("Test basic If Equal with strings", "[parser][If][Equal]")
{
    ParserTester parserTester("fun a() [if(\"abc\"==\"eee\")[return 1;]return 2;]");
    auto program = parserTester.parser.parseProgram();
    ParserVisitor visitor;
    REQUIRE(program != nullptr);
    REQUIRE(program->declarations.size() == 1);

    std::string expected = R"(Fun a()
 [
  if ("abc" Equal "eee")
   [
    return 1;
   ]
  return 2;
 ]
)";
    program->accept(visitor, 0);
    std::string programString = visitor.getParsedString();
    REQUIRE(programString == expected);
}

TEST_CASE("Test basic If Equal with ids", "[parser][If][Equal]")
{
    ParserTester parserTester("fun a() [var a = 1; const b = 2;\nif(a==b)[return 1;]return 2;]");
    auto program = parserTester.parser.parseProgram();
    ParserVisitor visitor;
    REQUIRE(program != nullptr);
    REQUIRE(program->declarations.size() == 1);

    std::string expected = R"(Fun a()
 [
  Var a = 1;
  Const b = 2;
  if (a Equal b)
   [
    return 1;
   ]
  return 2;
 ]
)";
    program->accept(visitor, 0);
    std::string programString = visitor.getParsedString();
    REQUIRE(programString == expected);
}

TEST_CASE("Test If with function call as condition", "[parser][If][GreaterEqual]")
{
    ParserTester parserTester("fun b()[return 1;]\nfun a() [if(b())[return 1;]return 2;]");
    auto program = parserTester.parser.parseProgram();
    ParserVisitor visitor;
    REQUIRE(program != nullptr);
    REQUIRE(program->declarations.size() == 2);

    std::string expected = R"(Fun b()
 [
  return 1;
 ]
Fun a()
 [
  if (b())
   [
    return 1;
   ]
  return 2;
 ]
)";
    program->accept(visitor, 0);
    std::string programString = visitor.getParsedString();
    REQUIRE(programString == expected);
}

TEST_CASE("Test If with else", "[parser][If][else]")
{
    ParserTester parserTester("fun a() [if(1<2)[return 1;]\nelse [return 2;]]");
    auto program = parserTester.parser.parseProgram();
    ParserVisitor visitor;
    REQUIRE(program != nullptr);
    REQUIRE(program->declarations.size() == 1);

    std::string expected = R"(Fun a()
 [
  if (1 Less 2)
   [
    return 1;
   ] else
   [
    return 2;
   ]
 ]
)";
    program->accept(visitor, 0);
    std::string programString = visitor.getParsedString();
    REQUIRE(programString == expected);
}

TEST_CASE("Test If with and", "[parser][If][and]")
{
    ParserTester parserTester("fun a() [if(1>2 && 2 > 3)[return 1;]return 2;]");
    auto program = parserTester.parser.parseProgram();
    ParserVisitor visitor;
    REQUIRE(program != nullptr);
    REQUIRE(program->declarations.size() == 1);

    std::string expected = R"(Fun a()
 [
  if (1 Greater 2 And 2 Greater 3)
   [
    return 1;
   ]
  return 2;
 ]
)";
    program->accept(visitor, 0);
    std::string programString = visitor.getParsedString();
    REQUIRE(programString == expected);
}

TEST_CASE("Test If with or", "[parser][If][or]")
{
    ParserTester parserTester("fun a() [if(1>2 || 2 > 3)[return 1;]return 2;]");
    auto program = parserTester.parser.parseProgram();
    ParserVisitor visitor;
    REQUIRE(program != nullptr);
    REQUIRE(program->declarations.size() == 1);

    std::string expected = R"(Fun a()
 [
  if (1 Greater 2 Or 2 Greater 3)
   [
    return 1;
   ]
  return 2;
 ]
)";
    program->accept(visitor, 0);
    std::string programString = visitor.getParsedString();
    REQUIRE(programString == expected);
}

TEST_CASE("Test empty if statement", "[parser][if]")
{
    ParserTester parserTester("fun a() [if(1<2)[]]");
    auto program = parserTester.parser.parseProgram();
    ParserVisitor visitor;
    REQUIRE(program != nullptr);

    std::string expected = R"(Fun a()
 [
  if (1 Less 2)
   [
   ]
 ]
)";
    program->accept(visitor, 0);
    std::string programString = visitor.getParsedString();
    REQUIRE(programString == expected);
}

TEST_CASE("Test If with more advanced logic", "[parser][If][or][and]")
{
    ParserTester parserTester("fun a() [ if(1<2 && 3>4 || 5==5)[return 1;]]");
    auto program = parserTester.parser.parseProgram();
    ParserVisitor visitor;
    REQUIRE(program != nullptr);
    REQUIRE(program->declarations.size() == 1);

    std::string expected = R"(Fun a()
 [
  if (1 Less 2 And 3 Greater 4 Or 5 Equal 5)
   [
    return 1;
   ]
 ]
)";
    program->accept(visitor, 0);
    std::string programString = visitor.getParsedString();
    REQUIRE(programString == expected);
}

TEST_CASE("Test basic While", "[parser][While][Less]")
{
    ParserTester parserTester("fun a() [while(1<2)[return 1;]return 2;]");
    auto program = parserTester.parser.parseProgram();
    ParserVisitor visitor;
    REQUIRE(program != nullptr);
    REQUIRE(program->declarations.size() == 1);

    std::string expected = R"(Fun a()
 [
  While (1 Less 2)
   [
    return 1;
   ]
  return 2;
 ]
)";
    program->accept(visitor, 0);
    std::string programString = visitor.getParsedString();
    REQUIRE(programString == expected);
}

TEST_CASE("Test basic While greater", "[parser][While][Greater]")
{
    ParserTester parserTester("fun a() [while(1>2)[return 1;]return 2;]");
    auto program = parserTester.parser.parseProgram();
    ParserVisitor visitor;
    REQUIRE(program != nullptr);
    REQUIRE(program->declarations.size() == 1);

    std::string expected = R"(Fun a()
 [
  While (1 Greater 2)
   [
    return 1;
   ]
  return 2;
 ]
)";
    program->accept(visitor, 0);
    std::string programString = visitor.getParsedString();
    REQUIRE(programString == expected);
}

TEST_CASE("Test basic While LessEqual", "[parser][While][LessEqual]")
{
    ParserTester parserTester("fun a() [while(1<=2)[return 1;]return 2;]");
    auto program = parserTester.parser.parseProgram();
    ParserVisitor visitor;
    REQUIRE(program != nullptr);
    REQUIRE(program->declarations.size() == 1);

    std::string expected = R"(Fun a()
 [
  While (1 LessEqual 2)
   [
    return 1;
   ]
  return 2;
 ]
)";
    program->accept(visitor, 0);
    std::string programString = visitor.getParsedString();
    REQUIRE(programString == expected);
}

TEST_CASE("Test basic While GreaterEqual", "[parser][While][GreaterEqual]")
{
    ParserTester parserTester("fun a() [while(1>=2)[return 1;]return 2;]");
    auto program = parserTester.parser.parseProgram();
    ParserVisitor visitor;
    REQUIRE(program != nullptr);
    REQUIRE(program->declarations.size() == 1);

    std::string expected = R"(Fun a()
 [
  While (1 GreaterEqual 2)
   [
    return 1;
   ]
  return 2;
 ]
)";
    program->accept(visitor, 0);
    std::string programString = visitor.getParsedString();
    REQUIRE(programString == expected);
}

TEST_CASE("Test basic While Equal", "[parser][While][Equal]")
{
    ParserTester parserTester("fun a() [while(1==2)[return 1;]return 2;]");
    auto program = parserTester.parser.parseProgram();
    ParserVisitor visitor;
    REQUIRE(program != nullptr);
    REQUIRE(program->declarations.size() == 1);

    std::string expected = R"(Fun a()
 [
  While (1 Equal 2)
   [
    return 1;
   ]
  return 2;
 ]
)";
    program->accept(visitor, 0);
    std::string programString = visitor.getParsedString();
    REQUIRE(programString == expected);
}

TEST_CASE("Test basic While Equal with strings", "[parser][While][Equal]")
{
    ParserTester parserTester("fun a() [while(\"abc\"==\"eee\")[return 1;]return 2;]");
    auto program = parserTester.parser.parseProgram();
    ParserVisitor visitor;
    REQUIRE(program != nullptr);
    REQUIRE(program->declarations.size() == 1);

    std::string expected = R"(Fun a()
 [
  While ("abc" Equal "eee")
   [
    return 1;
   ]
  return 2;
 ]
)";
    program->accept(visitor, 0);
    std::string programString = visitor.getParsedString();
    REQUIRE(programString == expected);
}

TEST_CASE("Test basic While Equal with ids", "[parser][While][Equal]")
{
    ParserTester parserTester("fun a() [var a = 1; const b = 2;\nwhile(a==b)[return 1;]return 2;]");
    auto program = parserTester.parser.parseProgram();
    ParserVisitor visitor;
    REQUIRE(program != nullptr);
    REQUIRE(program->declarations.size() == 1);

    std::string expected = R"(Fun a()
 [
  Var a = 1;
  Const b = 2;
  While (a Equal b)
   [
    return 1;
   ]
  return 2;
 ]
)";
    program->accept(visitor, 0);
    std::string programString = visitor.getParsedString();
    REQUIRE(programString == expected);
}

TEST_CASE("Test While with function call as condition", "[parser][While][GreaterEqual]")
{
    ParserTester parserTester("fun b()[return 1;]\nfun a() [while(b())[return 1;]return 2;]");
    auto program = parserTester.parser.parseProgram();
    ParserVisitor visitor;
    REQUIRE(program != nullptr);
    REQUIRE(program->declarations.size() == 2);

    std::string expected = R"(Fun b()
 [
  return 1;
 ]
Fun a()
 [
  While (b())
   [
    return 1;
   ]
  return 2;
 ]
)";
    program->accept(visitor, 0);
    std::string programString = visitor.getParsedString();
    REQUIRE(programString == expected);
}

TEST_CASE("Test while with and", "[parser][while][and]")
{
    ParserTester parserTester("fun a() [while(1>2 && 2 > 3)[return 1;]return 2;]");
    auto program = parserTester.parser.parseProgram();
    ParserVisitor visitor;
    REQUIRE(program != nullptr);
    REQUIRE(program->declarations.size() == 1);

    std::string expected = R"(Fun a()
 [
  While (1 Greater 2 And 2 Greater 3)
   [
    return 1;
   ]
  return 2;
 ]
)";
    program->accept(visitor, 0);
    std::string programString = visitor.getParsedString();
    REQUIRE(programString == expected);
}

TEST_CASE("Test while with or", "[parser][while][or]")
{
    ParserTester parserTester("fun a() [while(1>2 || 2 > 3)[return 1;]return 2;]");
    auto program = parserTester.parser.parseProgram();
    ParserVisitor visitor;
    REQUIRE(program != nullptr);
    REQUIRE(program->declarations.size() == 1);

    std::string expected = R"(Fun a()
 [
  While (1 Greater 2 Or 2 Greater 3)
   [
    return 1;
   ]
  return 2;
 ]
)";
    program->accept(visitor, 0);
    std::string programString = visitor.getParsedString();
    REQUIRE(programString == expected);
}

TEST_CASE("Test empty while statement", "[parser][while]")
{
    ParserTester parserTester("fun a() [while(1<2)[]]");
    auto program = parserTester.parser.parseProgram();
    ParserVisitor visitor;
    REQUIRE(program != nullptr);

    std::string expected = R"(Fun a()
 [
  While (1 Less 2)
   [
   ]
 ]
)";
    program->accept(visitor, 0);
    std::string programString = visitor.getParsedString();
    REQUIRE(programString == expected);
}

TEST_CASE("Test missing semicolon after declaration", "[parser][error]")
{
    ParserTester parserTester("var x");
    REQUIRE_THROWS_WITH(
        parserTester.parser.parseProgram(),
        "SemanticError at 1:6 → Expected ';' after declaration while parsing program");
}

TEST_CASE("Test missing parenthesis in function declaration", "[parser][error]")
{
    ParserTester parserTester("fun a) []");
    REQUIRE_THROWS_WITH(parserTester.parser.parseProgram(), "SemanticError at 1:6 → Expected '('");
}

TEST_CASE("Test invalid type in cast", "[parser][error][cast]")
{
    ParserTester parserTester("fun a() [var x = 1 as invalidType;]");
    REQUIRE_THROWS_WITH(parserTester.parser.parseProgram(),
                        "SemanticError at 1:23 → Expected a type");
}

TEST_CASE("Test invalid expression after operator", "[parser][error]")
{
    ParserTester parserTester("fun a() [var x = 1 + ;]");
    REQUIRE_THROWS_WITH(parserTester.parser.parseProgram(),
                        "SemanticError at 1:22 → Expected expression after operator while parsing "
                        "simpleExpression");
}

TEST_CASE("Test missing right bracket in statement block", "[parser][error]")
{
    ParserTester parserTester("fun a() [var x = 1;");
    REQUIRE_THROWS_WITH(parserTester.parser.parseProgram(), "SemanticError at 1:20 → Expected ']'");
}

TEST_CASE("Test missing left bracket in statement block", "[parser][error]")
{
    ParserTester parserTester("fun a() var x = 1;]");
    REQUIRE_THROWS_WITH(parserTester.parser.parseProgram(), "SemanticError at 1:9 → Expected '['");
}

TEST_CASE("Test missing logical expression in if", "[parser][error][if]")
{
    ParserTester parserTester("fun a() [var x = 1; if()[x = x + 1;]]");
    REQUIRE_THROWS_WITH(parserTester.parser.parseProgram(),
                        "SemanticError at 1:24 → Expected logical expression in if");
}

TEST_CASE("Test missing logical expression in while", "[parser][error][while]")
{
    ParserTester parserTester("fun a() [var x = 1; while()[x = x + 1;]]");
    REQUIRE_THROWS_WITH(parserTester.parser.parseProgram(),
                        "SemanticError at 1:27 → Expected logical expression in while");
}

TEST_CASE("Test empty program", "[parser]")
{
    ParserTester parserTester("");
    auto program = parserTester.parser.parseProgram();
    REQUIRE(program != nullptr);
    REQUIRE(program->declarations.empty());
}

TEST_CASE("Test nested blocks", "[parser][blocks]")
{
    ParserTester parserTester("fun a() [ [var x = 1;] ]");
    REQUIRE_THROWS_WITH(parserTester.parser.parseProgram(), "SemanticError at 1:11 → Expected ']'");
}

TEST_CASE("Test complex real-world example", "[parser][integration]")
{
    ParserTester parserTester(R"(
        fun factorial(var n) [
            if (n <= 1) [
                return 1;
            ]
            return n * factorial(n - 1);
        ]
        var x = factorial(5);
    )");
    auto program = parserTester.parser.parseProgram();
    ParserVisitor visitor;
    REQUIRE(program != nullptr);

    std::string expected = R"(Fun factorial(Var n)
 [
  if (n LessEqual 1)
   [
    return 1;
   ]
  return n Star factorial(n Minus 1);
 ]
Var x = factorial(5);
)";
    program->accept(visitor, 0);
    std::string programString = visitor.getParsedString();
    REQUIRE(programString == expected);
}
// Funkcje protected -> dziedziczenie i konwersja na publiczne
// Lub publiczne -> We wlasciwym dziedziczenie po publicznych zmiana na prywantne
// Mozna wziac drzewo AST i konwersja na string po czym sprawdzac w testach
