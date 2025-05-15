#pragma once

#include <string>

class AstNode;
class NumberLiteralNode;
class StringLiteralNode;
class IdentifierNode;
class BinaryOpNode;
class TypeCastNode;
class FunctionCallNode;
class ExpressionStatementNode;
class StatementBlockNode;
class FunctionDeclarationNode;
class FunctionLiteralNode;
class IfStatementNode;
class DeclarationNode;
class ReturnStatementNode;
class AssignNode;
class WhileStatementNode;
class ProgramNode;

class AstVisitor
{
   public:
    virtual ~AstVisitor() = default;
    virtual void visit(NumberLiteralNode& node) = 0;
    virtual void visit(StringLiteralNode& node) = 0;
    virtual void visit(IdentifierNode& node) = 0;
    virtual void visit(BinaryOpNode& node) = 0;
    virtual void visit(TypeCastNode& node) = 0;
    virtual void visit(FunctionCallNode& node) = 0;
    virtual void visit(ExpressionStatementNode& node) = 0;
    virtual void visit(StatementBlockNode& node) = 0;
    virtual void visit(FunctionDeclarationNode& node) = 0;
    virtual void visit(FunctionLiteralNode& node) = 0;
    virtual void visit(IfStatementNode& node) = 0;
    virtual void visit(DeclarationNode& node) = 0;
    virtual void visit(ReturnStatementNode& node) = 0;
    virtual void visit(AssignNode& node) = 0;
    virtual void visit(WhileStatementNode& node) = 0;
    virtual void visit(ProgramNode& node) = 0;
};