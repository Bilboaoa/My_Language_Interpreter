#include <string>
#include <unordered_map>
#include "asTree.hpp"

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

std::string NumberLiteralNode::toStr(int indent) const
{
    indent += 1;
    if (std::holds_alternative<int>(value))
    {
        return std::to_string(std::get<int>(value));
    }
    else if (std::holds_alternative<float>(value))
    {
        return std::to_string(std::get<float>(value));
    }
    return "<unknown number>";
}

std::string StringLiteralNode::toStr(int indent) const
{
    indent += 1;
    return "\"" + getValue() + "\"";
}

std::string IdentifierNode::toStr(int indent) const
{
    indent += 1;
    return getName();
}

std::string BinaryOpNode::toStr(int indent) const
{
    std::string result = "";
    result +=
        left->toStr(indent + 1) + " " + operatorToString(binOp) + " " + right->toStr(indent + 1);
    return result;
}

std::string TypeCastNode::toStr(int indent) const
{
    return expression->toStr(indent + 1) + " As " + typeToString(type);
}

std::string FunctionCallNode::toStr(int indent) const
{
    std::string out = callee->toStr(indent + 1) + "(";
    for (size_t i = 0; i < arguments.size(); ++i)
    {
        out += arguments[i]->toStr(indent + 1);
        if (i < arguments.size() - 1)
        {
            out += ", ";
        }
    }
    out += ")";
    return out;
}

std::string DeclarationNode::toStr(int indent) const
{
    std::string mod = modifier ? "Var" : "Const";
    std::string result = std::string(indent, ' ') + mod + " " + getIdentifierName();
    if (initializer)
    {
        result += " = " + initializer->toStr(indent + 1);
    }
    result += ";";
    return result;
}

std::string ExpressionStatementNode::toStr(int indent) const
{
    return std::string(indent, ' ') + expression->toStr(indent + 1) + ";";
}

std::string StatementBlockNode::toStr(int indent) const
{
    std::string result = std::string(indent, ' ') + "[\n";
    for (const auto& stmt : statements)
    {
        result += stmt->toStr(indent + 1) + "\n";
    }
    result += std::string(indent, ' ') + "]";
    return result;
}

std::string FunctionDeclarationNode::toStr(int indent) const
{
    std::string result = std::string(indent, ' ') + "Fun " + name + "(";
    for (size_t i = 0; i < params.size(); ++i)
    {
        std::string mod = params[i].modifier ? "Var" : "Const";
        result += mod + " " + params[i].id;
        if (i < params.size() - 1)
        {
            result += ", ";
        }
    }
    result += ")\n" + body->toStr(indent + 1);
    return result;
}

std::string FunctionLiteralNode::toStr(int indent) const
{
    std::string result = "Fun(";
    for (size_t i = 0; i < parameters.size(); ++i)
    {
        std::string mod = parameters[i].modifier ? "Var" : "Const";
        result += mod + " " + parameters[i].id;
        if (i < parameters.size() - 1)
        {
            result += ", ";
        }
    }
    result += ")\n" + body->toStr(indent + 1);
    return result;
}

std::string IfStatementNode::toStr(int indent) const
{
    std::string result = std::string(indent, ' ') + "if (" + condition->toStr(indent + 1) + ")\n" +
                         thenBlock->toStr(indent + 1);
    if (elseBlock)
    {
        result += " else\n" + elseBlock->toStr(indent + 1);
    }
    return result;
}

std::string ReturnStatementNode::toStr(int indent) const
{
    std::string result = std::string(indent, ' ') + "return";
    if (returnValue)
    {
        result += " " + returnValue->toStr(indent + 1);
    }
    result += ";";
    return result;
}

std::string AssignNode::toStr(int indent) const
{
    return std::string(indent, ' ') + identifier + " = " + expression->toStr(indent + 1) + ";";
}

std::string WhileStatementNode::toStr(int indent) const
{
    return std::string(indent, ' ') + "While (" + condition->toStr(indent + 1) + ")\n" +
           body->toStr(indent + 1);
}

std::string ProgramNode::toStr(int indent) const
{
    std::string result;
    for (const auto& node : declarations)
    {
        result += node->toStr(indent) + "\n";
    }
    return result;
}
