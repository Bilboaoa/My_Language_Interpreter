#include <iostream>
#include <memory>
#include <stdexcept>
#include <utility>
#include <algorithm>

#include "parser.hpp"

namespace
{
BinOperator getOperator(TokenType operatorType)
{
    switch (operatorType)
    {
        case TokenType::Plus:
            return BinOperator::Plus;
        case TokenType::Minus:
            return BinOperator::Minus;
        case TokenType::Star:
            return BinOperator::Star;
        case TokenType::Slash:
            return BinOperator::Slash;
        case TokenType::Equal:
            return BinOperator::Equal;
        case TokenType::NotEqual:
            return BinOperator::NotEqual;
        case TokenType::Greater:
            return BinOperator::Greater;
        case TokenType::GreaterEqual:
            return BinOperator::GreaterEqual;
        case TokenType::Less:
            return BinOperator::Less;
        case TokenType::LessEqual:
            return BinOperator::LessEqual;
        case TokenType::Pipe:
            return BinOperator::Pipe;
        case TokenType::AtAt:
            return BinOperator::AtAt;
        case TokenType::And:
            return BinOperator::And;
        case TokenType::Or:
            return BinOperator::Or;
        default:
            return BinOperator::Unknown;
    }
}

CastType getCastType(Token typeToken)
{
    std::string type = typeToken.getValue<std::string>();
    if (type == "string") return CastType::String;
    if (type == "float") return CastType::Float;
    if (type == "int") return CastType::Int;
    throw InterpreterException(ErrorType::Semantic, "Unexpected token type" + type,
                               typeToken.startPosition);
}

}  // namespace

Parser::Parser(Lexer& lexer) : lexer(lexer), currentToken(TokenType::Unknown, Position())
{
    advance();
}

Token Parser::advance()
{
    return std::exchange(currentToken, lexer.scanToken());
}

bool Parser::check(TokenType type) const
{
    return currentToken.type == type;
}

bool Parser::match(const std::vector<TokenType> types)
{
    bool found = std::find(types.begin(), types.end(), currentToken.type) != types.end();
    if (found) advance();
    return found;
}

bool Parser::isIn(const std::vector<TokenType> types)
{
    return std::find(types.begin(), types.end(), currentToken.type) != types.end();
}

Token Parser::consume(TokenType type, const std::string& errorMessage)
{
    if (check(type))
    {
        return advance();
    }
    throw error(errorMessage);
}

InterpreterException Parser::error(const std::string& message) const
{
    return InterpreterException(ErrorType::Semantic, message, currentToken.startPosition);
}

// Program         = { FunctionDeclaration | Declaration };
std::unique_ptr<ProgramNode> Parser::parseProgram()
{
    std::vector<std::unique_ptr<AstNode>> declarations;
    std::unique_ptr<DeclarationNode> declaration;
    std::unique_ptr<FunctionDeclarationNode> funcDeclaration;
    while ((funcDeclaration = parseFunctionDeclaration()) || (declaration = parseDeclaration()))
    {
        if (funcDeclaration != nullptr)
        {
            declarations.push_back(std::move(funcDeclaration));
        }
        else if (declaration != nullptr)
        {
            consume(TokenType::Semicolon, "Expected ';' after declaration while parsing program");
            declarations.push_back(std::move(declaration));
        }
        declaration = nullptr;
        funcDeclaration = nullptr;
    }
    if (!check(TokenType::EndOfFile)) throw error("Unexpected token in between declarations");
    return std::make_unique<ProgramNode>(std::move(declarations));
}

// FunctionDeclaration = “fun”, id, “(“, [ Parameters ], “)”, StatementBlock ;
std::unique_ptr<FunctionDeclarationNode> Parser::parseFunctionDeclaration()
{
    if (!check(TokenType::Fun)) return nullptr;
    Position startPos = currentToken.startPosition;
    consume(TokenType::Fun, "Expected 'fun'");
    std::string name =
        consume(TokenType::Identifier, "Expected function's name").getValue<std::string>();
    consume(TokenType::LParen, "Expected '('");
    std::vector<std::unique_ptr<FuncDefArgument>> params = parseParameters();
    consume(TokenType::RParen, "Expected ')'");
    std::unique_ptr<StatementBlockNode> body = parseStatementBlock();

    return std::make_unique<FunctionDeclarationNode>(name, startPos, std::move(params),
                                                     std::move(body));
}

// Parameters = Parameter, {“,”, Parameter }
std::vector<std::unique_ptr<FuncDefArgument>> Parser::parseParameters()
{
    std::vector<std::unique_ptr<FuncDefArgument>> params;
    auto param = parseParameter();
    if (!param)
    {
        return {};
    }
    params.push_back(std::move(param));
    while (match({TokenType::Comma}))
    {
        param = parseParameter();
        if (!param)
        {
            throw error("Expected 'const' or 'var'");
        }
        params.push_back(std::move(param));
    }
    return params;
}

// Parameter       = (“const” | “var”), id ;
std::unique_ptr<FuncDefArgument> Parser::parseParameter()
{
    if (!check({TokenType::Const}) && !check({TokenType::Var})) return nullptr;
    bool mod;
    if (match({TokenType::Const}))
    {
        mod = false;
    }
    else if (match({TokenType::Var}))
    {
        mod = true;
    }
    std::string tmpId =
        consume(TokenType::Identifier, "Expected param's name").getValue<std::string>();
    return std::make_unique<FuncDefArgument>(FuncDefArgument{mod, tmpId});
}

// StatementBlock = “[“, { Statement }, “]” ;
std::unique_ptr<StatementBlockNode> Parser::parseStatementBlock()
{
    consume(TokenType::LBracket, "Expected '['");
    Position startPos = currentToken.startPosition;
    std::vector<std::unique_ptr<StatementNode>> statements;
    while (!check(TokenType::RBracket) && !check(TokenType::EndOfFile))
    {
        statements.push_back(parseStatement());
    }
    consume(TokenType::RBracket, "Expected ']'");
    return std::make_unique<StatementBlockNode>(startPos, std::move(statements));
}

// Statement = IdOrCallAssign | IfStatement | Declaration, “;” | ReturnStatement, “;” |
// WhileStatement;
std::unique_ptr<StatementNode> Parser::parseStatement()
{
    if (auto ifStatement = parseIfStatement())
    {
        return ifStatement;
    }
    if (auto whileStatement = parseWhileStatement())
    {
        return whileStatement;
    }
    if (auto returnStatement = parseReturnStatement())
    {
        consume(TokenType::Semicolon, "Expected ';' after return");
        return returnStatement;
    }
    if (auto declaration = parseDeclaration())
    {
        consume(TokenType::Semicolon, "Expected ';' after declaration");
        return declaration;
    }
    if (auto idOrCall = parseIdOrCallAssign())
    {
        return idOrCall;
    }

    std::unique_ptr<ExpressionNode> expr = parseExpression();
    consume(TokenType::Semicolon, "Expected ';'");
    return std::make_unique<ExpressionStatementNode>(std::move(expr));
}

// IfStatement = “if”, “(“, LogicalExpr, “)”, StatementBlock, [“else”, StatementBlock] ;
std::unique_ptr<IfStatementNode> Parser::parseIfStatement()
{
    if (!match({TokenType::If})) return nullptr;
    const Position startPos = currentToken.startPosition;
    consume(TokenType::LParen, "Expected '('");
    std::unique_ptr<ExpressionNode> condition = parseLogicalExpr();
    consume(TokenType::RParen, "Expected ')'");
    std::unique_ptr<StatementBlockNode> thenBranch = parseStatementBlock();
    std::unique_ptr<StatementBlockNode> elseBranch = nullptr;
    if (match({TokenType::Else}))
    {
        elseBranch = parseStatementBlock();
    }
    return std::make_unique<IfStatementNode>(startPos, std::move(condition), std::move(thenBranch),
                                             std::move(elseBranch));
}

// WhileStatement = “while”, “(“, LogicalExpr, “)”, StatementBlock ;
std::unique_ptr<WhileStatementNode> Parser::parseWhileStatement()
{
    if (!check(TokenType::While)) return nullptr;
    Position startPos = currentToken.startPosition;
    consume(TokenType::While, "Expected 'while'");
    consume(TokenType::LParen, "Expected '('");
    std::unique_ptr<ExpressionNode> condition = parseLogicalExpr();
    consume(TokenType::RParen, "Expected ')'");
    std::unique_ptr<StatementBlockNode> body = parseStatementBlock();
    return std::make_unique<WhileStatementNode>(startPos, std::move(condition), std::move(body));
}

// ReturnStatement = “return”, [ Expression ];
std::unique_ptr<ReturnStatementNode> Parser::parseReturnStatement()
{
    if (!check(TokenType::Return)) return nullptr;
    Position startPos = currentToken.startPosition;
    consume(TokenType::Return, "Expected 'return'");
    std::unique_ptr<ExpressionNode> returnValue = nullptr;
    if (!check(TokenType::Semicolon))
    {
        returnValue = parseExpression();
    }
    return std::make_unique<ReturnStatementNode>(startPos, std::move(returnValue));
}

// Declaration = (“var” | “const var”), id, [“=”, Expression] ;
std::unique_ptr<DeclarationNode> Parser::parseDeclaration()
{
    if (!check(TokenType::Const) && !check(TokenType::Var)) return nullptr;
    const Position pos = currentToken.startPosition;
    const bool isVar = match({TokenType::Var});
    const bool isConst = !isVar && match({TokenType::Const});
    if (!isVar && !isConst)
    {
        throw error("Expected 'const' or 'var'");
    }

    std::string name =
        consume(TokenType::Identifier, "Expected variable's name").getValue<std::string>();
    std::unique_ptr<ExpressionNode> initializer = nullptr;
    if (match({TokenType::Assign}))
    {
        initializer = parseExpression();
    }
    return std::make_unique<DeclarationNode>(isVar, std::move(name), pos, std::move(initializer));
}

// IdOrCallAssign = id, PossibleAssignOrCall ;
std::unique_ptr<StatementNode> Parser::parseIdOrCallAssign()
{
    if (!check(TokenType::Identifier)) return nullptr;
    std::string id = consume(TokenType::Identifier, "Expected identifier").getValue<std::string>();
    return parsePossibleAssignOrCall(std::move(id));
}

// PossibleAssignOrCall = "=" Expression ";" | [ CallArguments ] ";" ;
std::unique_ptr<StatementNode> Parser::parsePossibleAssignOrCall(std::string id)
{
    Position startPos = currentToken.startPosition;
    if (match({TokenType::Assign}))
    {
        std::unique_ptr<ExpressionNode> expr = parseExpression();
        std::unique_ptr<AssignNode> assigned =
            std::make_unique<AssignNode>(id, startPos, std::move(expr));
        consume(TokenType::Semicolon, "No semicolon after assign");
        return assigned;
    }

    std::unique_ptr<ExpressionNode> callee = std::make_unique<IdentifierNode>(id, startPos);
    std::unique_ptr<ExpressionNode> call = parseFunctionCall(std::move(callee));
    std::unique_ptr<ExpressionStatementNode> node =
        std::make_unique<ExpressionStatementNode>(std::move(call));
    consume(TokenType::Semicolon, "No semicolon after call");
    return node;
}

// CallArguments   = “(“, [ ArgumentList ], “)” ;
std::unique_ptr<ExpressionNode> Parser::parseFunctionCall(std::unique_ptr<ExpressionNode> callee)
{
    consume(TokenType::LParen, "Expected '('");
    std::vector<std::unique_ptr<ExpressionNode>> args = parseArgumentList();
    consume(TokenType::RParen, "Expected ')'");
    return std::make_unique<FunctionCallNode>(std::move(callee), std::move(args));
}

// ArgumentList    = Expression, { “,”, Expression } ;
std::vector<std::unique_ptr<ExpressionNode>> Parser::parseArgumentList()
{
    std::vector<std::unique_ptr<ExpressionNode>> args;
    if (!check(TokenType::RParen))
    {
        do
        {
            args.push_back(parseExpression());
        } while (match({TokenType::Comma}));
    }
    return args;
}

// Expression = TypeCastExpression ;
// TypeCastExpression = SimpleExpression, { “as”, Type } ;
std::unique_ptr<ExpressionNode> Parser::parseExpression()
{
    std::unique_ptr<ExpressionNode> left = parseSimpleExpression();

    while (match({TokenType::As}))
    {
        Token typeToken = consume(TokenType::Type, "Expected a type");
        CastType type = getCastType(typeToken);
        left = std::make_unique<TypeCastNode>(std::move(left), type);
    }

    return left;
}

// LogicalExpr   = RelExpression, { LogicalExpr, RelExpression } ;
std::unique_ptr<ExpressionNode> Parser::parseLogicalExpr()
{
    std::unique_ptr<ExpressionNode> left = parseRelExpression();
    while (isIn({TokenType::And, TokenType::Or}))
    {
        Token operatorToken = advance();
        BinOperator op = getOperator(operatorToken.type);
        std::unique_ptr<ExpressionNode> right = parseRelExpression();
        left = std::make_unique<BinaryOpNode>(std::move(left), op, std::move(right));
    }
    return left;
}

// RelExpression   = Expression, { RelOperator, Expression } ;
std::unique_ptr<ExpressionNode> Parser::parseRelExpression()
{
    std::unique_ptr<ExpressionNode> left = parseSimpleExpression();
    while (isIn({TokenType::Equal, TokenType::NotEqual, TokenType::Greater, TokenType::GreaterEqual,
                 TokenType::Less, TokenType::LessEqual}))
    {
        Token operatorToken = advance();
        BinOperator op = getOperator(operatorToken.type);
        std::unique_ptr<ExpressionNode> right = parseSimpleExpression();
        left = std::make_unique<BinaryOpNode>(std::move(left), op, std::move(right));
    }
    return left;
}

// SimpleExpression  = Term, {("+" | "-" | "|" | "@@"), Term ;
std::unique_ptr<ExpressionNode> Parser::parseSimpleExpression()
{
    std::unique_ptr<ExpressionNode> left = parseTerm();
    while (isIn({TokenType::Plus, TokenType::Minus, TokenType::Pipe, TokenType::AtAt}))
    {
        Token operatorToken = advance();
        BinOperator op = getOperator(operatorToken.type);
        std::unique_ptr<ExpressionNode> right = parseTerm();
        left = std::make_unique<BinaryOpNode>(std::move(left), op, std::move(right));
    }
    return left;
}

// Term      = Factor, { (“*” | “/”) Factor }
std::unique_ptr<ExpressionNode> Parser::parseTerm()
{
    std::unique_ptr<ExpressionNode> left = parseFactor();
    while (isIn({TokenType::Star, TokenType::Slash}))
    {
        Token operatorToken = advance();
        BinOperator op = getOperator(operatorToken.type);
        std::unique_ptr<ExpressionNode> right = parseFactor();
        left = std::make_unique<BinaryOpNode>(std::move(left), op, std::move(right));
    }
    return left;
}

// Factor = BaseFactor PossibleCallArguments ;
std::unique_ptr<ExpressionNode> Parser::parseFactor()
{
    std::unique_ptr<ExpressionNode> expr = parseBaseFactor();
    expr = parsePossibleCallArguments(std::move(expr));
    return expr;
}

// PossibleCallArguments = { CallArguments } ;
std::unique_ptr<ExpressionNode> Parser::parsePossibleCallArguments(
    std::unique_ptr<ExpressionNode> expr)
{
    while (check(TokenType::LParen))
    {
        expr = parseFunctionCall(std::move(expr));
    }
    return expr;
}

// BaseFactor = Number | LiteralString | id | “(“, Expression, “)” | FunctionLiteral;
std::unique_ptr<ExpressionNode> Parser::parseBaseFactor()
{
    if (check(TokenType::Number))
    {
        Token numToken = consume(TokenType::Number, "Expected a number");

        if (auto intValue = std::get_if<int>(&numToken.value))
            return std::make_unique<NumberLiteralNode>(*intValue, numToken.startPosition);

        return std::make_unique<NumberLiteralNode>(std::get<float>(numToken.value),
                                                   numToken.startPosition);
    }
    if (check(TokenType::StringLiteral))
    {
        Token str = consume(TokenType::StringLiteral, "Expected string literal");
        std::string literal = str.getValue<std::string>();
        return std::make_unique<StringLiteralNode>(literal, str.startPosition);
    }
    if (check(TokenType::Identifier))
    {
        Position startPos = currentToken.startPosition;
        std::string id =
            consume(TokenType::Identifier, "Expected an identification").getValue<std::string>();
        return std::make_unique<IdentifierNode>(id, startPos);
    }
    if (check(TokenType::LParen))
    {
        consume(TokenType::LParen, "Expected '(' while parsing expression");
        std::unique_ptr<ExpressionNode> expr = parseExpression();
        consume(TokenType::RParen, "Expected ')' while parsing expression");
        return expr;
    }
    if (auto funcLiteral = parseFunctionLiteral())
    {
        return funcLiteral;
    }
    throw error("Expected an expression");
}

// FunctionLiteral = "fun", "(", [ Parameters ], ")", StatementBlock ;
std::unique_ptr<FunctionLiteralNode> Parser::parseFunctionLiteral()
{
    if (!check(TokenType::Fun)) return nullptr;
    Position startPos = currentToken.startPosition;
    Token funToken = consume(TokenType::Fun, "Expected 'fun'");
    Token lParenToken = consume(TokenType::LParen, "Expected '('");
    std::vector<std::unique_ptr<FuncDefArgument>> parameters = parseParameters();
    Token rParenToken = consume(TokenType::RParen, "Expected ')'");
    std::unique_ptr<StatementBlockNode> body = parseStatementBlock();

    return std::make_unique<FunctionLiteralNode>(startPos, std::move(parameters), std::move(body));
}