#include <string>
#include <unordered_map>
#include "asTree.hpp"

void NumberLiteralNode::accept(AstVisitor& visitor, int indent)
{
    return visitor.visit(*this, indent);
}

void StringLiteralNode::accept(AstVisitor& visitor, int indent)
{
    return visitor.visit(*this, indent);
}

void IdentifierNode::accept(AstVisitor& visitor, int indent)
{
    return visitor.visit(*this, indent);
}

void BinaryOpNode::accept(AstVisitor& visitor, int indent)
{
    return visitor.visit(*this, indent);
}

void TypeCastNode::accept(AstVisitor& visitor, int indent)
{
    return visitor.visit(*this, indent);
}

void FunctionCallNode::accept(AstVisitor& visitor, int indent)
{
    return visitor.visit(*this, indent);
}

void DeclarationNode::accept(AstVisitor& visitor, int indent)
{
    return visitor.visit(*this, indent);
}

void ExpressionStatementNode::accept(AstVisitor& visitor, int indent)
{
    return visitor.visit(*this, indent);
}

void StatementBlockNode::accept(AstVisitor& visitor, int indent)
{
    return visitor.visit(*this, indent);
}

void FunctionDeclarationNode::accept(AstVisitor& visitor, int indent)
{
    return visitor.visit(*this, indent);
}

void FunctionLiteralNode::accept(AstVisitor& visitor, int indent)
{
    return visitor.visit(*this, indent);
}

void IfStatementNode::accept(AstVisitor& visitor, int indent)
{
    return visitor.visit(*this, indent);
}

void ReturnStatementNode::accept(AstVisitor& visitor, int indent)
{
    return visitor.visit(*this, indent);
}

void AssignNode::accept(AstVisitor& visitor, int indent)
{
    return visitor.visit(*this, indent);
}

void WhileStatementNode::accept(AstVisitor& visitor, int indent)
{
    return visitor.visit(*this, indent);
}

void ProgramNode::accept(AstVisitor& visitor, int indent)
{
    return visitor.visit(*this, indent);
}
