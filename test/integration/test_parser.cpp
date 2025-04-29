#include <memory>

#include "catch2/catch_all.hpp"

#include "asTree.hpp"
#include "parser.hpp"
#include "error.hpp"

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

    REQUIRE(program != nullptr);
    REQUIRE(program->declarations.size() == 1);

    std::string expected = "Var x;\n";
    std::string programString = program->toStr(0);
    REQUIRE(programString == expected);
}

TEST_CASE("Test Minimal const Declaration", "[parser][declaration]")
{
    ParserTester parserTester("const x;");
    auto program = parserTester.parser.parseProgram();

    REQUIRE(program != nullptr);
    REQUIRE(program->declarations.size() == 1);

    std::string expected = "Const x;\n";
    std::string programString = program->toStr(0);
    REQUIRE(programString == expected);
}

TEST_CASE("Test multiple Minimal const Declaration", "[parser][declaration]")
{
    ParserTester parserTester("const x; var z; const y;");
    auto program = parserTester.parser.parseProgram();

    REQUIRE(program != nullptr);
    REQUIRE(program->declarations.size() == 3);

    std::string expected = "Const x;\nVar z;\nConst y;\n";
    std::string programString = program->toStr(0);
    REQUIRE(programString == expected);
}

TEST_CASE("Test Minimal function Declaration", "[parser][function]")
{
    ParserTester parserTester("fun a() []");
    auto program = parserTester.parser.parseProgram();
    REQUIRE(program != nullptr);
    REQUIRE(program->declarations.size() == 1);

    std::string expected = "Fun a()\n [\n ]\n";
    std::string programString = program->toStr(0);
    REQUIRE(programString == expected);
}

TEST_CASE("Test Minimal function Declaration with body", "[parser][function][assign]")
{
    ParserTester parserTester("fun a() [var a=2;]");
    auto program = parserTester.parser.parseProgram();
    REQUIRE(program != nullptr);
    REQUIRE(program->declarations.size() == 1);

    std::string expected = "Fun a()\n [\n  Var a = 2;\n ]\n";
    std::string programString = program->toStr(0);
    REQUIRE(programString == expected);
}

TEST_CASE("Test Minimal function Declaration with body and one argument",
          "[parser][function][assign]")
{
    ParserTester parserTester("fun a(var a) [a=2;]");
    auto program = parserTester.parser.parseProgram();
    REQUIRE(program != nullptr);
    REQUIRE(program->declarations.size() == 1);

    std::string expected = "Fun a(Var a)\n [\n  a = 2;\n ]\n";
    std::string programString = program->toStr(0);
    REQUIRE(programString == expected);
}

TEST_CASE("Test Minimal function Declaration with body and many arguments",
          "[parser][function][assign]")
{
    ParserTester parserTester("fun a(var a, const b, var c) [a=2;]");
    auto program = parserTester.parser.parseProgram();
    REQUIRE(program != nullptr);
    REQUIRE(program->declarations.size() == 1);

    std::string expected = "Fun a(Var a, Const b, Var c)\n [\n  a = 2;\n ]\n";
    std::string programString = program->toStr(0);
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

TEST_CASE("Test assign with float", "[parser][assign]")
{
    ParserTester parserTester("var a=2.1;");
    auto program = parserTester.parser.parseProgram();
    REQUIRE(program != nullptr);
    REQUIRE(program->declarations.size() == 1);

    std::string expected = "Var a = 2.100000;\n";
    std::string programString = program->toStr(0);
    REQUIRE(programString == expected);
}

TEST_CASE("Test assign with string literal", "[parser][assign][string]")
{
    ParserTester parserTester("var a=\"abc\";");
    auto program = parserTester.parser.parseProgram();
    REQUIRE(program != nullptr);
    REQUIRE(program->declarations.size() == 1);

    std::string expected = "Var a = \"abc\";\n";
    std::string programString = program->toStr(0);
    REQUIRE(programString == expected);
}

TEST_CASE("Test assign with function literal (no args)", "[parser][assign][function]")
{
    ParserTester parserTester("var a=fun()[return 1;];");
    auto program = parserTester.parser.parseProgram();
    REQUIRE(program != nullptr);
    REQUIRE(program->declarations.size() == 1);

    std::string expected = R"(Var a = Fun()
  [
   return 1;
  ];
)";
    std::string programString = program->toStr(0);
    REQUIRE(programString == expected);
}

TEST_CASE("Test assign with function literal (with args)", "[parser][assign][function]")
{
    ParserTester parserTester("var a=fun(var a, const b)[return a;];");
    auto program = parserTester.parser.parseProgram();
    REQUIRE(program != nullptr);
    REQUIRE(program->declarations.size() == 1);

    std::string expected = R"(Var a = Fun(Var a, Const b)
  [
   return a;
  ];
)";
    std::string programString = program->toStr(0);
    REQUIRE(programString == expected);
}

TEST_CASE("Test assign after declaration", "[parser][function][assign]")
{
    ParserTester parserTester("fun a() [var a; a=2;]");
    auto program = parserTester.parser.parseProgram();
    REQUIRE(program != nullptr);
    REQUIRE(program->declarations.size() == 1);

    std::string expected = "Fun a()\n [\n  Var a;\n  a = 2;\n ]\n";
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

TEST_CASE("Test function declaration with add", "[parser][function][plus]")
{
    ParserTester parserTester("fun a() [var a=2+2;]");
    auto program = parserTester.parser.parseProgram();
    REQUIRE(program != nullptr);
    REQUIRE(program->declarations.size() == 1);

    std::string expected = "Fun a()\n [\n  Var a = 2 Plus 2;\n ]\n";
    std::string programString = program->toStr(0);
    REQUIRE(programString == expected);
}

TEST_CASE("Test function declaration with sub", "[parser][function][minus]")
{
    ParserTester parserTester("fun a() [var a=2-2;]");
    auto program = parserTester.parser.parseProgram();
    REQUIRE(program != nullptr);
    REQUIRE(program->declarations.size() == 1);

    std::string expected = "Fun a()\n [\n  Var a = 2 Minus 2;\n ]\n";
    std::string programString = program->toStr(0);
    REQUIRE(programString == expected);
}

TEST_CASE("Test function declaration with mult", "[parser][function][star]")
{
    ParserTester parserTester("fun a() [var a=2*2;]");
    auto program = parserTester.parser.parseProgram();
    REQUIRE(program != nullptr);
    REQUIRE(program->declarations.size() == 1);

    std::string expected = "Fun a()\n [\n  Var a = 2 Star 2;\n ]\n";
    std::string programString = program->toStr(0);
    REQUIRE(programString == expected);
}

TEST_CASE("Test function declaration with division", "[parser][function][slash]")
{
    ParserTester parserTester("fun a() [var a=2/2;]");
    auto program = parserTester.parser.parseProgram();
    REQUIRE(program != nullptr);
    REQUIRE(program->declarations.size() == 1);

    std::string expected = "Fun a()\n [\n  Var a = 2 Slash 2;\n ]\n";
    std::string programString = program->toStr(0);
    REQUIRE(programString == expected);
}

TEST_CASE("Test function declaration with @@", "[parser][function][AtAt]")
{
    ParserTester parserTester("fun a() [var a=2@@1;]");
    auto program = parserTester.parser.parseProgram();
    REQUIRE(program != nullptr);
    REQUIRE(program->declarations.size() == 1);

    std::string expected = "Fun a()\n [\n  Var a = 2 AtAt 1;\n ]\n";
    std::string programString = program->toStr(0);
    REQUIRE(programString == expected);
}

TEST_CASE("Test function declaration with |", "[parser][function][Pipe]")
{
    ParserTester parserTester("fun a() [var a=2|1;]");
    auto program = parserTester.parser.parseProgram();
    REQUIRE(program != nullptr);
    REQUIRE(program->declarations.size() == 1);

    std::string expected = "Fun a()\n [\n  Var a = 2 Pipe 1;\n ]\n";
    std::string programString = program->toStr(0);
    REQUIRE(programString == expected);
}

TEST_CASE("Test expression with 2 functions", "[parser][function][Pipe]")
{
    ParserTester parserTester(
        "fun a() [return 1;]\n fun b() [return 2;]\n fun c() [var d=a()|b();]");
    auto program = parserTester.parser.parseProgram();
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
    std::string programString = program->toStr(0);
    REQUIRE(programString == expected);
}

TEST_CASE("Test declaration with assigned string concatination", "[parser][Plus]")
{
    ParserTester parserTester("var a=\"abc\"+\"de\";");
    auto program = parserTester.parser.parseProgram();
    REQUIRE(program != nullptr);
    REQUIRE(program->declarations.size() == 1);

    std::string expected = "Var a = \"abc\" Plus \"de\";\n";
    std::string programString = program->toStr(0);
    REQUIRE(programString == expected);
}

TEST_CASE("Test basic If", "[parser][If][Less]")
{
    ParserTester parserTester("fun a() [if(1<2)[return 1;]return 2;]");
    auto program = parserTester.parser.parseProgram();
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
    std::string programString = program->toStr(0);
    REQUIRE(programString == expected);
}

TEST_CASE("Test basic If greater", "[parser][If][Greater]")
{
    ParserTester parserTester("fun a() [if(1>2)[return 1;]return 2;]");
    auto program = parserTester.parser.parseProgram();
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
    std::string programString = program->toStr(0);
    REQUIRE(programString == expected);
}

TEST_CASE("Test basic If LessEqual", "[parser][If][LessEqual]")
{
    ParserTester parserTester("fun a() [if(1<=2)[return 1;]return 2;]");
    auto program = parserTester.parser.parseProgram();
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
    std::string programString = program->toStr(0);
    REQUIRE(programString == expected);
}

TEST_CASE("Test basic If GreaterEqual", "[parser][If][GreaterEqual]")
{
    ParserTester parserTester("fun a() [if(1>=2)[return 1;]return 2;]");
    auto program = parserTester.parser.parseProgram();
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
    std::string programString = program->toStr(0);
    REQUIRE(programString == expected);
}

TEST_CASE("Test If with function call as condition", "[parser][If][GreaterEqual]")
{
    ParserTester parserTester("fun b()[return 1;]\nfun a() [if(b())[return 1;]return 2;]");
    auto program = parserTester.parser.parseProgram();
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
    std::string programString = program->toStr(0);
    REQUIRE(programString == expected);
}

TEST_CASE("Test If with else", "[parser][If][else]")
{
    ParserTester parserTester("fun a() [if(1<2)[return 1;]\nelse [return 2;]]");
    auto program = parserTester.parser.parseProgram();
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
    std::string programString = program->toStr(0);
    REQUIRE(programString == expected);
}

TEST_CASE("Test If with and", "[parser][If][and]")
{
    ParserTester parserTester("fun a() [if(1>2 && 2 > 3)[return 1;]return 2;]");
    auto program = parserTester.parser.parseProgram();
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
    std::string programString = program->toStr(0);
    REQUIRE(programString == expected);
}

TEST_CASE("Test If with or", "[parser][If][or]")
{
    ParserTester parserTester("fun a() [if(1>2 || 2 > 3)[return 1;]return 2;]");
    auto program = parserTester.parser.parseProgram();
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
    std::string programString = program->toStr(0);
    REQUIRE(programString == expected);
}
// Funkcje protected -> dziedziczenie i konwersja na publiczne
// Lub publiczne -> We wlasciwym dziedziczenie po publicznych zmiana na prywantne
// Mozna wziac drzewo AST i konwersja na string po czym sprawdzac w testach
