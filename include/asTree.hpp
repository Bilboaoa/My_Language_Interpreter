#pragma once

#include <vector>
#include <memory>
#include <optional>
#include "token.hpp"
#include "position.hpp"
#include "interpreter_exception.hpp"

struct FuncDefArgument
{
    Token modifierToken;
    Token id;
};

class ExpressionNode;
class StatementNode;

class AstNode
{
   public:
    virtual ~AstNode() = default;
    virtual Position getStartPosition() const = 0;
};

class ExpressionNode : public AstNode
{
};

class NumberLiteralNode : public ExpressionNode
{
   public:
    Token token;
    NumberLiteralNode(Token token) : token(token) {}
    Position getStartPosition() const override { return token.startPosition; }
    int getValueInt() const { return token.getValue<int>(); }
    float getValueFloat() const { return token.getValue<float>(); }
};

class StringLiteralNode : public ExpressionNode
{
   public:
    Token token;
    StringLiteralNode(Token token) : token(token) {}
    Position getStartPosition() const override { return token.startPosition; }
    std::string getValue() const { return token.getValue<std::string>(); }
};

class IdentifierNode : public ExpressionNode
{
   public:
    Token token;
    IdentifierNode(Token token) : token(token) {}
    Position getStartPosition() const override { return token.startPosition; }
    std::string getName() const { return token.getValue<std::string>(); }
};

class BinaryOpNode : public ExpressionNode
{
   public:
    std::unique_ptr<ExpressionNode> left;
    Token opToken;
    std::unique_ptr<ExpressionNode> right;
    BinaryOpNode(std::unique_ptr<ExpressionNode> left, Token opToken,
                 std::unique_ptr<ExpressionNode> right)
        : left(std::move(left)), opToken(opToken), right(std::move(right))
    {
    }
    Position getStartPosition() const override { return left->getStartPosition(); }
};

class TypeCastNode : public ExpressionNode
{
   public:
    std::unique_ptr<ExpressionNode> expression;
    Token typeToken;
    TypeCastNode(std::unique_ptr<ExpressionNode> expression, Token typeToken)
        : expression(std::move(expression)), typeToken(typeToken)
    {
    }
    Position getStartPosition() const override { return expression->getStartPosition(); }
    TokenType getTargetType() const { return typeToken.type; }
};

class FunctionCallNode : public ExpressionNode
{
   public:
    std::unique_ptr<ExpressionNode> callee;
    std::vector<std::unique_ptr<ExpressionNode>> arguments;
    FunctionCallNode(std::unique_ptr<ExpressionNode> callee,
                     std::vector<std::unique_ptr<ExpressionNode>> arguments)
        : callee(std::move(callee)), arguments(std::move(arguments))
    {
    }
    Position getStartPosition() const override { return callee->getStartPosition(); }
};

class StatementNode : public AstNode
{
};

class ExpressionStatementNode : public StatementNode
{
   public:
    std::unique_ptr<ExpressionNode> expression;

    ExpressionStatementNode(std::unique_ptr<ExpressionNode> expression)
        : expression(std::move(expression))
    {
    }

    Position getStartPosition() const override { return expression->getStartPosition(); }
};

class StatementBlockNode : public StatementNode
{
   public:
    std::vector<std::unique_ptr<StatementNode>> statements;
    Token lBracketToken;
    Token rBracketToken;
    StatementBlockNode(Token lBracketToken, std::vector<std::unique_ptr<StatementNode>> statements,
                       Token rBracketToken)
        : statements(std::move(statements)),
          lBracketToken(lBracketToken),
          rBracketToken(rBracketToken)
    {
    }
    Position getStartPosition() const override { return lBracketToken.startPosition; }
};

class FunctionDeclarationNode : public AstNode
{
   public:
    Token name;
    std::vector<FuncDefArgument> params;
    std::unique_ptr<StatementBlockNode> body;
    FunctionDeclarationNode(Token n, std::vector<FuncDefArgument> param,
                            std::unique_ptr<StatementBlockNode> bod)
        : name(n), params(param), body(std::move(bod))
    {
    }
    Position getStartPosition() const override { return name.startPosition; }
};

class FunctionLiteralNode : public ExpressionNode
{
   public:
    std::vector<FuncDefArgument> parameters;
    std::unique_ptr<StatementBlockNode> body;
    Token funToken;
    Token lParenToken;
    Token rParenToken;
    FunctionLiteralNode(Token funToken, Token lParenToken, std::vector<FuncDefArgument> parameters,
                        Token rParenToken, std::unique_ptr<StatementBlockNode> body)
        : parameters(std::move(parameters)),
          body(std::move(body)),
          funToken(funToken),
          lParenToken(lParenToken),
          rParenToken(rParenToken)
    {
    }
    Position getStartPosition() const override { return funToken.startPosition; }
};

class IfStatementNode : public StatementNode
{
   public:
    std::unique_ptr<ExpressionNode> condition;
    std::unique_ptr<StatementBlockNode> thenBlock;
    std::unique_ptr<StatementBlockNode> elseBlock;
    Token ifToken;
    Token lParenToken;
    Token rParenToken;
    Token elseToken;
    IfStatementNode(Token ifToken, Token lParenToken, std::unique_ptr<ExpressionNode> condition,
                    Token rParenToken, std::unique_ptr<StatementBlockNode> thenBlock,
                    std::unique_ptr<StatementBlockNode> elseBlock = nullptr,
                    Token elseToken = Token(TokenType::Unknown, Position()))
        : condition(std::move(condition)),
          thenBlock(std::move(thenBlock)),
          elseBlock(std::move(elseBlock)),
          ifToken(ifToken),
          lParenToken(lParenToken),
          rParenToken(rParenToken),
          elseToken(elseToken)
    {
    }
    Position getStartPosition() const override { return ifToken.startPosition; }
};

class DeclarationNode : public StatementNode
{
   public:
    Token typeModifierToken;
    Token identifierToken;
    std::unique_ptr<ExpressionNode> initializer;
    Token assignToken;
    DeclarationNode(Token typeModifierToken, Token identifierToken,
                    std::unique_ptr<ExpressionNode> initializer = nullptr,
                    Token assignToken = Token(TokenType::Unknown, Position()))
        : typeModifierToken(typeModifierToken),
          identifierToken(identifierToken),
          initializer(std::move(initializer)),
          assignToken(assignToken)
    {
    }
    Position getStartPosition() const override { return typeModifierToken.startPosition; }
    std::string getIdentifierName() const { return identifierToken.getValue<std::string>(); }
};

class ReturnStatementNode : public StatementNode
{
   public:
    Token returnToken;
    std::unique_ptr<ExpressionNode> returnValue;
    ReturnStatementNode(Token returnToken, std::unique_ptr<ExpressionNode> returnValue = nullptr)
        : returnToken(returnToken), returnValue(std::move(returnValue))
    {
    }
    Position getStartPosition() const override { return returnToken.startPosition; }
};

class AssignNode : public StatementNode
{
   public:
    Token identifierToken;
    Token assignToken;
    std::unique_ptr<ExpressionNode> expression;
    AssignNode(Token identifierToken, Token assignToken, std::unique_ptr<ExpressionNode> expression)
        : identifierToken(identifierToken),
          assignToken(assignToken),
          expression(std::move(expression))
    {
    }
    Position getStartPosition() const override { return identifierToken.startPosition; }
    std::string getIdentifierName() const { return identifierToken.getValue<std::string>(); }
};

class WhileStatementNode : public StatementNode
{
   public:
    std::unique_ptr<ExpressionNode> condition;
    std::unique_ptr<StatementBlockNode> body;
    Token whileToken;
    Token lParenToken;
    Token rParenToken;
    WhileStatementNode(Token whileToken, Token lParenToken,
                       std::unique_ptr<ExpressionNode> condition, Token rParenToken,
                       std::unique_ptr<StatementBlockNode> body)
        : condition(std::move(condition)),
          body(std::move(body)),
          whileToken(whileToken),
          lParenToken(lParenToken),
          rParenToken(rParenToken)
    {
    }
    Position getStartPosition() const override { return whileToken.startPosition; }
};

class ProgramNode : public AstNode
{
   public:
    std::vector<std::unique_ptr<AstNode>> declarations;
    ProgramNode(std::vector<std::unique_ptr<AstNode>> declarations)
        : declarations(std::move(declarations))
    {
    }
    Position getStartPosition() const override
    {
        return declarations.empty() ? Position() : declarations.front()->getStartPosition();
    }
};
