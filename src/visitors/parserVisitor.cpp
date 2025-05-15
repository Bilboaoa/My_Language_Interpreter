#include "parserVisitor.hpp"
#include <string>
#include <variant>

namespace
{
std::string operatorToString(BinOperator op)
{
    switch (op)
    {
        case BinOperator::Plus:
            return "Plus";
        case BinOperator::Minus:
            return "Minus";
        case BinOperator::Star:
            return "Star";
        case BinOperator::Slash:
            return "Slash";
        case BinOperator::Equal:
            return "Equal";
        case BinOperator::NotEqual:
            return "NotEqual";
        case BinOperator::Greater:
            return "Greater";
        case BinOperator::GreaterEqual:
            return "GreaterEqual";
        case BinOperator::Less:
            return "Less";
        case BinOperator::LessEqual:
            return "LessEqual";
        case BinOperator::Pipe:
            return "Pipe";
        case BinOperator::AtAt:
            return "AtAt";
        case BinOperator::And:
            return "And";
        case BinOperator::Or:
            return "Or";
        default:
            return "Wrong TokenType";
    }
}

std::string typeToString(CastType type)
{
    switch (type)
    {
        case CastType::String:
            return "string";
        case CastType::Float:
            return "float";
        case CastType::Int:
            return "int";
        default:
            return "Wrong Type";
    }
}

}  // namespace

void ParserVisitor::visit(NumberLiteralNode& node, int indent)
{
    indent++;
    outcome.push_back(std::visit([](auto arg) { return std::to_string(arg); }, node.getValue()));
}

void ParserVisitor::visit(StringLiteralNode& node, int indent)
{
    indent++;
    outcome.push_back("\"" + node.getValue() + "\"");
}

void ParserVisitor::visit(IdentifierNode& node, int indent)
{
    indent++;
    outcome.push_back(node.getName());
}

void ParserVisitor::visit(BinaryOpNode& node, int indent)
{
    std::string result = "";
    node.left->accept(*this, indent + 1);
    outcome.push_back(" " + operatorToString(node.getBinOp()) + " ");
    node.right->accept(*this, indent + 1);
}

void ParserVisitor::visit(TypeCastNode& node, int indent)
{
    node.expression->accept(*this, indent + 1);
    outcome.push_back(" As " + typeToString(node.getTargetType()));
}

void ParserVisitor::visit(FunctionCallNode& node, int indent)
{
    node.callee->accept(*this, indent + 1);
    outcome.push_back("(");
    for (size_t i = 0; i < node.arguments.size(); ++i)
    {
        node.arguments[i]->accept(*this, indent + 1);
        if (i < node.arguments.size() - 1)
        {
            outcome.push_back(", ");
        }
    }
    outcome.push_back(")");
}

void ParserVisitor::visit(ExpressionStatementNode& node, int indent)
{
    outcome.push_back(std::string(indent, ' '));
    node.expression->accept(*this, indent + 1);
    outcome.push_back(";");
}

void ParserVisitor::visit(StatementBlockNode& node, int indent)
{
    outcome.push_back(std::string(indent, ' ') + "[\n");
    for (const auto& stmt : node.statements)
    {
        stmt->accept(*this, indent + 1);
        outcome.push_back("\n");
    }
    outcome.push_back(std::string(indent, ' ') + "]");
}

void ParserVisitor::visit(FunctionDeclarationNode& node, int indent)
{
    outcome.push_back(std::string(indent, ' ') + "Fun " + node.getName() + "(");
    for (size_t i = 0; i < node.params.size(); ++i)
    {
        std::string paramBlock;
        std::string mod = node.params[i]->modifier ? "Var" : "Const";
        paramBlock += mod + " " + node.params[i]->id;
        if (i < node.params.size() - 1)
        {
            paramBlock += ", ";
        }
        outcome.push_back(paramBlock);
    }
    outcome.push_back(")\n");
    node.body->accept(*this, indent + 1);
}

void ParserVisitor::visit(FunctionLiteralNode& node, int indent)
{
    outcome.push_back("Fun(");
    for (size_t i = 0; i < node.parameters.size(); ++i)
    {
        std::string paramBlock;
        std::string mod = node.parameters[i]->modifier ? "Var" : "Const";
        paramBlock += mod + " " + node.parameters[i]->id;
        if (i < node.parameters.size() - 1)
        {
            paramBlock += ", ";
        }
        outcome.push_back(paramBlock);
    }
    outcome.push_back(")\n");
    node.body->accept(*this, indent + 1);
}

void ParserVisitor::visit(IfStatementNode& node, int indent)
{
    outcome.push_back(std::string(indent, ' ') + "if (");
    node.condition->accept(*this, indent + 1);
    outcome.push_back(")\n"); 
    node.thenBlock->accept(*this, indent + 1);

    if (node.elseBlock)
    {
        outcome.push_back(" else\n");
        node.elseBlock->accept(*this, indent + 1);
    }
}

void ParserVisitor::visit(DeclarationNode& node, int indent)
{
    std::string mod = node.getModifier() ? "Var" : "Const";
    std::string result = std::string(indent, ' ') + mod + " " + node.getIdentifierName();
    outcome.push_back(result);
    if (node.initializer)
    {
        outcome.push_back(" = ");
        node.initializer->accept(*this, indent + 1);
    }
    outcome.push_back(";");
    
}

void ParserVisitor::visit(ReturnStatementNode& node, int indent)
{
    outcome.push_back(std::string(indent, ' ') + "return");
    if (node.returnValue)
    {
        outcome.push_back(" ");
        node.returnValue->accept(*this, indent + 1);
    }
    outcome.push_back(";");
}

void ParserVisitor::visit(AssignNode& node, int indent)
{
    outcome.push_back(std::string(indent, ' ') + node.getIdentifierName() + " = ");
    node.expression->accept(*this, indent + 1); 
    outcome.push_back(";");
}

void ParserVisitor::visit(WhileStatementNode& node, int indent)
{
    outcome.push_back(std::string(indent, ' ') + "While ("); 
    node.condition->accept(*this, indent + 1);
    outcome.push_back(")\n");
    node.body->accept(*this, indent + 1);
}

void ParserVisitor::visit(ProgramNode& node, int indent)
{
    for (const auto& decl : node.declarations)
    {
        decl->accept(*this, indent);
        outcome.push_back("\n");
    }
}

std::string ParserVisitor::getParsedString() const
{
    std::string parsedString = "";
    for (std::string str : outcome)
        parsedString += str;
    return parsedString;
}