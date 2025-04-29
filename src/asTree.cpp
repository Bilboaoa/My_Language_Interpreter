#include <string>
#include <unordered_map>
#include "asTree.hpp"

namespace
{
std::string tokenTypeToString(TokenType type)
{
    switch (type)
    {
        case TokenType::Identifier:
            return "Identifier";
        case TokenType::Number:
            return "Number";
        case TokenType::StringLiteral:
            return "StringLiteral";
        case TokenType::Type:
            return "Type";
        case TokenType::Var:
            return "Var";
        case TokenType::Const:
            return "Const";
        case TokenType::Fun:
            return "Fun";
        case TokenType::Return:
            return "Return";
        case TokenType::If:
            return "If";
        case TokenType::Else:
            return "Else";
        case TokenType::While:
            return "While";
        case TokenType::As:
            return "As";
        case TokenType::Print:
            return "Print";
        case TokenType::Plus:
            return "Plus";
        case TokenType::Minus:
            return "Minus";
        case TokenType::Star:
            return "Star";
        case TokenType::Slash:
            return "Slash";
        case TokenType::Equal:
            return "Equal";
        case TokenType::NotEqual:
            return "NotEqual";
        case TokenType::Greater:
            return "Greater";
        case TokenType::GreaterEqual:
            return "GreaterEqual";
        case TokenType::Less:
            return "Less";
        case TokenType::LessEqual:
            return "LessEqual";
        case TokenType::Pipe:
            return "Pipe";
        case TokenType::AtAt:
            return "AtAt";
        case TokenType::Assign:
            return "Assign";
        case TokenType::And:
            return "And";
        case TokenType::Or:
            return "Or";
        case TokenType::LParen:
            return "LParen";
        case TokenType::RParen:
            return "RParen";
        case TokenType::LBracket:
            return "LBracket";
        case TokenType::RBracket:
            return "RBracket";
        case TokenType::Semicolon:
            return "Semicolon";
        case TokenType::Comma:
            return "Comma";
        case TokenType::EndOfFile:
            return "EndOfFile";
        case TokenType::Unknown:
            return "Unknown";
        default:
            return "Unknown";
    }
}
}  // namespace

std::string NumberLiteralNode::toStr(int indent) const
{
    indent += 1;
    if (std::holds_alternative<int>(token.value))
    {
        return std::to_string(token.getValue<int>());
    }
    else if (std::holds_alternative<float>(token.value))
    {
        return std::to_string(token.getValue<float>());
    }
    return "";
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
    result += left->toStr(indent + 1) + " " + tokenTypeToString(opToken.type) + " " +
              right->toStr(indent + 1);
    return result;
}

std::string TypeCastNode::toStr(int indent) const
{
    return std::string(indent, ' ') + "(" + expression->toStr(indent + 1) + " as " +
           tokenTypeToString(typeToken.type) + ")";
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
    std::string result = std::string(indent, ' ') + tokenTypeToString(typeModifierToken.type) +
                         " " + getIdentifierName();
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
    std::string result = std::string(indent, ' ') + "Fun " + name.getValue<std::string>() + "(";
    for (size_t i = 0; i < params.size(); ++i)
    {
        result += tokenTypeToString(params[i].modifierToken.type) + " " +
                  params[i].id.getValue<std::string>();
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
        result += tokenTypeToString(parameters[i].modifierToken.type) + " " +
                  parameters[i].id.getValue<std::string>();
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
    return std::string(indent, ' ') + identifierToken.getValue<std::string>() + " = " +
           expression->toStr(indent + 1) + ";";
}

std::string WhileStatementNode::toStr(int indent) const
{
    return std::string(indent, ' ') + "while (" + condition->toStr(indent + 1) + ") " +
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
