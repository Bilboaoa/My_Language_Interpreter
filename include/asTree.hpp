#pragma once

#include <vector>
#include <memory>
#include <optional>
#include "token.hpp"
#include "position.hpp"
#include "interpreter_exception.hpp"

struct FuncDefArgument
{
    bool modifier;
    std::string id;
};

enum class BinOperator
{
    Plus,
    Minus,
    Star,
    Slash,
    Equal,
    NotEqual,
    Greater,
    GreaterEqual,
    Less,
    LessEqual,
    Pipe,
    AtAt,
    And,
    Or,
    Unknown
};

enum class CastType
{
    String,
    Float,
    Int
};

class ExpressionNode;
class StatementNode;

class AstNode
{
   public:
    virtual ~AstNode() = default;
    virtual Position getStartPosition() const = 0;
    virtual std::string toStr(int indent = 0) const = 0;
};

class ExpressionNode : public AstNode
{
};

class NumberLiteralNode : public ExpressionNode
{
    std::variant<int, float> value;
    Position pos;

   public:
    NumberLiteralNode(std::variant<int, float> val, Position p) : value(val), pos(p) {}
    Position getStartPosition() const override { return pos; }
    std::string toStr(int indent = 0) const override;
    std::variant<int, float> getValue() const { return value; }
};

class StringLiteralNode : public ExpressionNode
{
    std::string val;
    Position pos;

   public:
    StringLiteralNode(std::string v, Position p) : val(v), pos(p) {}
    Position getStartPosition() const override { return pos; }
    std::string toStr(int indent = 0) const override;
    std::string getValue() const { return val; }
};

class IdentifierNode : public ExpressionNode
{
    std::string name;
    Position pos;

   public:
    IdentifierNode(std::string n, Position p) : name(n), pos(p) {}
    Position getStartPosition() const override { return pos; }
    std::string toStr(int indent = 0) const override;
    std::string getName() const { return name; }
};

class BinaryOpNode : public ExpressionNode
{
    BinOperator binOp;

   public:
    std::unique_ptr<ExpressionNode> left;
    std::unique_ptr<ExpressionNode> right;
    BinaryOpNode(std::unique_ptr<ExpressionNode> left, BinOperator op,
                 std::unique_ptr<ExpressionNode> right)
        : binOp(op), left(std::move(left)), right(std::move(right))
    {
    }
    Position getStartPosition() const override { return left->getStartPosition(); }
    BinOperator getBinOp() const { return binOp; }
    std::string toStr(int indent = 0) const override;
};

class TypeCastNode : public ExpressionNode
{
    CastType type;

   public:
    std::unique_ptr<ExpressionNode> expression;
    TypeCastNode(std::unique_ptr<ExpressionNode> expression, CastType t)
        : type(t), expression(std::move(expression))
    {
    }
    Position getStartPosition() const override { return expression->getStartPosition(); }
    std::string toStr(int indent = 0) const override;
    CastType getTargetType() const { return type; }
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
    std::string toStr(int indent = 0) const override;
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
    std::string toStr(int indent = 0) const override;
};

class StatementBlockNode : public StatementNode
{
    Position pos;

   public:
    std::vector<std::unique_ptr<StatementNode>> statements;
    StatementBlockNode(Position p, std::vector<std::unique_ptr<StatementNode>> statements)
        : pos(p), statements(std::move(statements))
    {
    }
    Position getStartPosition() const override { return pos; }
    std::string toStr(int indent) const override;
};

class FunctionDeclarationNode : public AstNode
{
    std::string name;
    Position pos;

   public:
    std::vector<std::unique_ptr<FuncDefArgument>> params;
    std::unique_ptr<StatementBlockNode> body;
    FunctionDeclarationNode(std::string n, Position p,
                            std::vector<std::unique_ptr<FuncDefArgument>> param,
                            std::unique_ptr<StatementBlockNode> bod)
        : name(n), pos(p), params(std::move(param)), body(std::move(bod))
    {
    }
    Position getStartPosition() const override { return pos; }
    std::string getName() const { return name; }
    std::string toStr(int indent) const override;
};

class FunctionLiteralNode : public ExpressionNode
{
    Position pos;

   public:
    std::vector<std::unique_ptr<FuncDefArgument>> parameters;
    std::unique_ptr<StatementBlockNode> body;
    FunctionLiteralNode(Position p, std::vector<std::unique_ptr<FuncDefArgument>> parameters,
                        std::unique_ptr<StatementBlockNode> body)
        : pos(p), parameters(std::move(parameters)), body(std::move(body))
    {
    }
    Position getStartPosition() const override { return pos; }
    std::string toStr(int indent) const override;
};

class IfStatementNode : public StatementNode
{
    Position pos;

   public:
    std::unique_ptr<ExpressionNode> condition;
    std::unique_ptr<StatementBlockNode> thenBlock;
    std::unique_ptr<StatementBlockNode> elseBlock;
    IfStatementNode(Position p, std::unique_ptr<ExpressionNode> condition,
                    std::unique_ptr<StatementBlockNode> thenBlock,
                    std::unique_ptr<StatementBlockNode> elseBlock = nullptr)
        : pos(p),
          condition(std::move(condition)),
          thenBlock(std::move(thenBlock)),
          elseBlock(std::move(elseBlock))
    {
    }
    Position getStartPosition() const override { return pos; }
    std::string toStr(int indent) const override;
};

class DeclarationNode : public StatementNode
{
    bool modifier;
    std::string identifier;
    Position pos;

   public:
    std::unique_ptr<ExpressionNode> initializer;
    DeclarationNode(bool m, std::string i, Position p,
                    std::unique_ptr<ExpressionNode> initializer = nullptr)
        : modifier(m), identifier(i), pos(p), initializer(std::move(initializer))
    {
    }
    Position getStartPosition() const override { return pos; }
    std::string getIdentifierName() const { return identifier; }
    bool getModifier() { return modifier; }
    std::string toStr(int indent) const override;
};

class ReturnStatementNode : public StatementNode
{
    Position pos;

   public:
    std::unique_ptr<ExpressionNode> returnValue;
    ReturnStatementNode(Position p, std::unique_ptr<ExpressionNode> returnValue = nullptr)
        : pos(p), returnValue(std::move(returnValue))
    {
    }
    Position getStartPosition() const override { return pos; }
    std::string toStr(int indent) const override;
};

class AssignNode : public StatementNode
{
    std::string identifier;
    Position pos;

   public:
    std::unique_ptr<ExpressionNode> expression;
    AssignNode(std::string i, Position p, std::unique_ptr<ExpressionNode> expression)
        : identifier(i), pos(p), expression(std::move(expression))
    {
    }
    Position getStartPosition() const override { return pos; }
    std::string toStr(int indent) const override;
    std::string getIdentifierName() const { return identifier; }
};

class WhileStatementNode : public StatementNode
{
    Position pos;

   public:
    std::unique_ptr<ExpressionNode> condition;
    std::unique_ptr<StatementBlockNode> body;
    WhileStatementNode(Position p, std::unique_ptr<ExpressionNode> condition,
                       std::unique_ptr<StatementBlockNode> body)
        : pos(p), condition(std::move(condition)), body(std::move(body))
    {
    }
    Position getStartPosition() const override { return pos; }
    std::string toStr(int indent) const override;
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
    std::string toStr(int indent) const override;
};
