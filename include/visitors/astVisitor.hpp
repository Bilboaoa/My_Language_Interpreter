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
    virtual void visit(NumberLiteralNode& node, int indent) = 0;
    virtual void visit(StringLiteralNode& node, int indent) = 0;
    virtual void visit(IdentifierNode& node, int indent) = 0;
    virtual void visit(BinaryOpNode& node, int indent) = 0;
    virtual void visit(TypeCastNode& node, int indent) = 0;
    virtual void visit(FunctionCallNode& node, int indent) = 0;
    virtual void visit(ExpressionStatementNode& node, int indent) = 0;
    virtual void visit(StatementBlockNode& node, int indent) = 0;
    virtual void visit(FunctionDeclarationNode& node, int indent) = 0;
    virtual void visit(FunctionLiteralNode& node, int indent) = 0;
    virtual void visit(IfStatementNode& node, int indent) = 0;
    virtual void visit(DeclarationNode& node, int indent) = 0;
    virtual void visit(ReturnStatementNode& node, int indent) = 0;
    virtual void visit(AssignNode& node, int indent) = 0;
    virtual void visit(WhileStatementNode& node, int indent) = 0;
    virtual void visit(ProgramNode& node, int indent) = 0;
};