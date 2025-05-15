#include "parserVisitor.hpp"
#include <string>
#include <variant>
#include <iostream> // Dodane dla debugowania

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

std::string indent(int level)
{
    return std::string(level, ' ');
}

}  // namespace

void ParserVisitor::visit(NumberLiteralNode& node)
{
    outcome.push_back(std::visit([](auto arg)
                                { return std::to_string(arg); },
                                node.getValue()));
}

void ParserVisitor::visit(StringLiteralNode& node)
{
    outcome.push_back("\"" + node.getValue() + "\"");
}

void ParserVisitor::visit(IdentifierNode& node)
{
    outcome.push_back(node.getName());
}

void ParserVisitor::visit(BinaryOpNode& node)
{
    node.left->accept(*this);
    outcome.push_back(" " + operatorToString(node.getBinOp()) + " ");
    node.right->accept(*this);
}

void ParserVisitor::visit(TypeCastNode& node)
{
    node.expression->accept(*this);
    outcome.push_back(" As " + typeToString(node.getTargetType()));
}

void ParserVisitor::visit(FunctionCallNode& node)
{
    node.callee->accept(*this);
    outcome.push_back("(");
    for (size_t i = 0; i < node.arguments.size(); ++i)
    {
        node.arguments[i]->accept(*this);
        if (i < node.arguments.size() - 1)
        {
            outcome.push_back(", ");
        }
    }
    outcome.push_back(")");
}

void ParserVisitor::visit(ExpressionStatementNode& node)
{
    node.expression->accept(*this);
    outcome.push_back(";");
}

void ParserVisitor::visit(StatementBlockNode& node)
{
    outcome.push_back("[\n");
    indentation++;
    for (const auto& stmt : node.statements)
    {
        outcome.push_back(indent(indentation));
        stmt->accept(*this);
        outcome.push_back("\n");
    }
    indentation--;
    outcome.push_back(indent(indentation) + "]");
}

void ParserVisitor::visit(FunctionDeclarationNode& node)
{
    outcome.push_back("Fun " + node.getName() + "(");
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
    indentation++;
    outcome.push_back(indent(indentation));
    node.body->accept(*this);
    indentation--;
}

void ParserVisitor::visit(FunctionLiteralNode& node)
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
    indentation++;
    outcome.push_back(indent(indentation));
    node.body->accept(*this);
    indentation--;
}

void ParserVisitor::visit(IfStatementNode& node)
{
    outcome.push_back("if (");
    node.condition->accept(*this);
    outcome.push_back(")\n");
    indentation++;
    outcome.push_back(indent(indentation));
    node.thenBlock->accept(*this);
    indentation--;

    if (node.elseBlock)
    {
        outcome.push_back(" else\n");
        indentation++;
        outcome.push_back(indent(indentation));
        node.elseBlock->accept(*this);
        indentation--;
    }
}

void ParserVisitor::visit(DeclarationNode& node)
{
    std::string mod = node.getModifier() ? "Var" : "Const";
    std::string result = mod + " " + node.getIdentifierName();
    outcome.push_back(result);
    if (node.initializer)
    {
        outcome.push_back(" = ");
        node.initializer->accept(*this);
    }
    outcome.push_back(";");
}

void ParserVisitor::visit(ReturnStatementNode& node)
{
    outcome.push_back("return");
    if (node.returnValue)
    {
        outcome.push_back(" ");
        node.returnValue->accept(*this);
    }
    outcome.push_back(";");
}

void ParserVisitor::visit(AssignNode& node)
{
    outcome.push_back(node.getIdentifierName() + " = ");
    node.expression->accept(*this);
    outcome.push_back(";");
}

void ParserVisitor::visit(WhileStatementNode& node)
{
    outcome.push_back("While (");
    node.condition->accept(*this);
    outcome.push_back(")\n");
    indentation++;
    outcome.push_back(indent(indentation));
    node.body->accept(*this);
    indentation--;
}

void ParserVisitor::visit(ProgramNode& node)
{
    for (const auto& decl : node.declarations)
    {
        decl->accept(*this);
        outcome.push_back("\n");
    }
}

std::string ParserVisitor::getParsedString() const
{
    std::string parsedString = "";
    for (const std::string& str : outcome)
    {
        parsedString += str;
    }
    return parsedString;
}