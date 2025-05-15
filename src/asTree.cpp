#include <string>
#include <unordered_map>
#include "asTree.hpp"

void NumberLiteralNode::accept(AstVisitor& visitor)
{
    return visitor.visit(*this);
}

void StringLiteralNode::accept(AstVisitor& visitor)
{
    return visitor.visit(*this);
}

void IdentifierNode::accept(AstVisitor& visitor)
{
    return visitor.visit(*this);
}

void BinaryOpNode::accept(AstVisitor& visitor)
{
    return visitor.visit(*this);
}

void TypeCastNode::accept(AstVisitor& visitor)
{
    return visitor.visit(*this);
}

void FunctionCallNode::accept(AstVisitor& visitor)
{
    return visitor.visit(*this);
}

void DeclarationNode::accept(AstVisitor& visitor)
{
    return visitor.visit(*this);
}

void ExpressionStatementNode::accept(AstVisitor& visitor)
{
    return visitor.visit(*this);
}

void StatementBlockNode::accept(AstVisitor& visitor)
{
    return visitor.visit(*this);
}

void FunctionDeclarationNode::accept(AstVisitor& visitor)
{
    return visitor.visit(*this);
}

void FunctionLiteralNode::accept(AstVisitor& visitor)
{
    return visitor.visit(*this);
}

void IfStatementNode::accept(AstVisitor& visitor)
{
    return visitor.visit(*this);
}

void ReturnStatementNode::accept(AstVisitor& visitor)
{
    return visitor.visit(*this);
}

void AssignNode::accept(AstVisitor& visitor)
{
    return visitor.visit(*this);
}

void WhileStatementNode::accept(AstVisitor& visitor)
{
    return visitor.visit(*this);
}

void ProgramNode::accept(AstVisitor& visitor)
{
    return visitor.visit(*this);
}
