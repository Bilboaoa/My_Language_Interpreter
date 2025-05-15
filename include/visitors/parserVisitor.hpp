#pragma once
#include "astVisitor.hpp"
#include "asTree.hpp"
#include <string>
#include <variant>

class ParserVisitor : public AstVisitor
{
   protected:
    std::vector<std::string> outcome;

    void visit(NumberLiteralNode& node, int indent) override;
    void visit(StringLiteralNode& node, int indent) override;
    void visit(IdentifierNode& node, int indent) override;
    void visit(BinaryOpNode& node, int indent) override;
    void visit(TypeCastNode& node, int indent) override;
    void visit(FunctionCallNode& node, int indent) override;
    void visit(ExpressionStatementNode& node, int indent) override;
    void visit(StatementBlockNode& node, int indent) override;
    void visit(FunctionDeclarationNode& node, int indent) override;
    void visit(FunctionLiteralNode& node, int indent) override;
    void visit(IfStatementNode& node, int indent) override;
    void visit(DeclarationNode& node, int indent) override;
    void visit(ReturnStatementNode& node, int indent) override;
    void visit(AssignNode& node, int indent) override;
    void visit(WhileStatementNode& node, int indent) override;

   public:
    void visit(ProgramNode& node, int indent) override;
    std::string getParsedString() const;
};