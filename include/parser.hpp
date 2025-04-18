#include <vector>
#include <memory>
#include <optional>

#include "lexer.hpp"
#include "token.hpp"
#include "position.hpp"
#include "interpreter_exception.hpp"
#include "asTree.hpp"
#include "error.hpp"

class Parser
{
   public:
    explicit Parser(Lexer& lexer);
    std::unique_ptr<ProgramNode> parseProgram();

   private:
    Lexer& lexer;
    Token currentToken;

    Token advance();
    bool check(TokenType type) const;
    bool match(std::vector<TokenType>);
    Token consume(TokenType type, const std::string& errorMessage);
    InterpreterException error(const std::string& message) const;
    std::unique_ptr<FunctionDeclarationNode> parseFunctionDeclaration();
    std::vector<FuncDefArgument> parseParameters();
    std::unique_ptr<StatementBlockNode> parseStatementBlock();
    std::unique_ptr<StatementNode> parseStatement();
    std::unique_ptr<IfStatementNode> parseIfStatement();
    std::unique_ptr<WhileStatementNode> parseWhileStatement();
    std::unique_ptr<ReturnStatementNode> parseReturnStatement();
    std::unique_ptr<DeclarationNode> parseDeclaration();
    std::unique_ptr<AssignNode> parseAssign();
    std::unique_ptr<ExpressionNode> parseExpression();
    std::unique_ptr<ExpressionNode> parseTypeCastExpression();
    std::unique_ptr<ExpressionNode> parseLogicalExpr();
    std::unique_ptr<ExpressionNode> parseRelExpression();
    std::unique_ptr<ExpressionNode> parseSimpleExpression();
    std::unique_ptr<ExpressionNode> parseTerm();
    std::unique_ptr<ExpressionNode> parseFactor();
    std::unique_ptr<ExpressionNode> parseFunctionCall(std::unique_ptr<ExpressionNode> callee);
    std::vector<std::unique_ptr<ExpressionNode>> parseArgumentList();
    std::unique_ptr<ExpressionNode> parseBaseFactor();
    std::unique_ptr<FunctionLiteralNode> parseFunctionLiteral();
};