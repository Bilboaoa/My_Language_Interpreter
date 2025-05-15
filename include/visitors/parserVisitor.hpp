#pragma once
#include "astVisitor.hpp"
#include "asTree.hpp"
#include <string>
#include <variant>

class ParserVisitor : public AstVisitor
{
public:
    void visit(ProgramNode& node) override;
    std::string getParsedString() const;

protected:
    std::vector<std::string> outcome;
    int indentation = 0;

    void visit(NumberLiteralNode& node) override;
    void visit(StringLiteralNode& node) override;
    void visit(IdentifierNode& node) override;
    void visit(BinaryOpNode& node) override;
    void visit(TypeCastNode& node) override;
    void visit(FunctionCallNode& node) override;
    void visit(ExpressionStatementNode& node) override;
    void visit(StatementBlockNode& node) override;
    void visit(FunctionDeclarationNode& node) override;
    void visit(FunctionLiteralNode& node) override;
    void visit(IfStatementNode& node) override;
    void visit(DeclarationNode& node) override;
    void visit(ReturnStatementNode& node) override;
    void visit(AssignNode& node) override;
    void visit(WhileStatementNode& node) override;
};