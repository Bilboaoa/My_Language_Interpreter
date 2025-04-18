#include <iostream>
#include <memory>
#include <stdexcept>

#include "parser.hpp"

Parser::Parser(Lexer& lexer) : lexer(lexer), currentToken(TokenType::Unknown, Position())
{
    advance();
}

Token Parser::advance()
{
    Token previous = currentToken;
    currentToken = lexer.scanToken();
    return previous;
}

bool Parser::check(TokenType type) const
{
    return currentToken.type == type;
}

bool Parser::match(std::vector<TokenType> types)
{
    for (TokenType type : types)
    {
        if (check(type))
        {
            advance();
            return true;
        }
    }
    return false;
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

std::unique_ptr<ProgramNode> Parser::parseProgram()
{
    std::vector<std::unique_ptr<AstNode>> declarations;
    while (!check(TokenType::EndOfFile))
    {
        if (check(TokenType::Fun))
        {
            declarations.push_back(parseFunctionDeclaration());
        }
        declarations.push_back(parseDeclaration());
    }
    return std::make_unique<ProgramNode>(std::move(declarations));
}

std::unique_ptr<FunctionDeclarationNode> Parser::parseFunctionDeclaration()
{
    consume(TokenType::Fun, "Expected 'fun'");
    Token name = consume(TokenType::Identifier, "Expected function's name");
    consume(TokenType::LParen, "Expected '('");
    std::vector<FuncDefArgument> params = parseParameters();
    consume(TokenType::RParen, "Expected ')'");
    std::unique_ptr<StatementBlockNode> body = parseStatementBlock();

    return std::make_unique<FunctionDeclarationNode>(name, params, std::move(body));
}

std::vector<FuncDefArgument> Parser::parseParameters()
{
    std::vector<FuncDefArgument> params;
    if (!check(TokenType::RParen))
    {
        do
        {
            Token tmp(TokenType::Unknown, currentToken.startPosition);
            if (match({TokenType::Const}))
            {
                tmp = consume(TokenType::Var, "Expected 'var'");
            }
            else if (check(TokenType::Var))
            {
                tmp = consume(TokenType::Var, "Expected 'var'");
            }
            Token tmpId = consume(TokenType::Identifier, "Expected param's name");
            params.push_back(FuncDefArgument{tmp, tmpId});
        } while (match({TokenType::Comma}));
    }
    return params;
}

std::unique_ptr<StatementBlockNode> Parser::parseStatementBlock()
{
    consume(TokenType::LBracket, "Expected '['");
    std::vector<std::unique_ptr<StatementNode>> statements;
    while (!check(TokenType::RBracket) && !check(TokenType::EndOfFile))
    {
        statements.push_back(parseStatement());
    }
    consume(TokenType::RBracket, "Expected ']'");
    return std::make_unique<StatementBlockNode>(currentToken, std::move(statements), currentToken);
}

std::unique_ptr<StatementNode> Parser::parseStatement()
{
    if (check(TokenType::If))
    {
        return parseIfStatement();
    }
    if (check(TokenType::While))
    {
        return parseWhileStatement();
    }
    if (check(TokenType::Return))
    {
        return parseReturnStatement();
    }
    if (check(TokenType::Var) || check(TokenType::Const))
    {
        return parseDeclaration();
    }
    if (check(TokenType::Identifier))
    {
        // Jak wykonać to bez peek
        if (check(TokenType::LParen))
        {
            std::unique_ptr<ExpressionNode> call = parseFactor();
            consume(TokenType::Semicolon, "Expected ';'");
            return std::make_unique<ExpressionStatementNode>(std::move(call));
        }
        else if (check(TokenType::Assign)) 
        {
            return parseAssign();
        }
    }

    std::unique_ptr<ExpressionNode> expr = parseExpression();
    consume(TokenType::Semicolon, "Expected ';'");
    return std::make_unique<ExpressionStatementNode>(std::move(expr));
}

std::unique_ptr<IfStatementNode> Parser::parseIfStatement()
{
    consume(TokenType::If, "Expected 'if'");
    consume(TokenType::LParen, "Expected '('");
    std::unique_ptr<ExpressionNode> condition = parseLogicalExpr();
    consume(TokenType::RParen, "Expected ')'");
    std::unique_ptr<StatementBlockNode> thenBranch = parseStatementBlock();
    std::unique_ptr<StatementBlockNode> elseBranch = nullptr;
    if (match({TokenType::Else}))
    {
        elseBranch = parseStatementBlock();
    }
    return std::make_unique<IfStatementNode>(currentToken, currentToken, std::move(condition),
                                             currentToken, std::move(thenBranch),
                                             std::move(elseBranch), currentToken);
}

std::unique_ptr<WhileStatementNode> Parser::parseWhileStatement()
{
    consume(TokenType::While, "Expected 'while'");
    consume(TokenType::LParen, "Expected '('");
    std::unique_ptr<ExpressionNode> condition = parseLogicalExpr();
    consume(TokenType::RParen, "Expected ')'");
    std::unique_ptr<StatementBlockNode> body = parseStatementBlock();
    return std::make_unique<WhileStatementNode>(currentToken, currentToken, std::move(condition),
                                                currentToken, std::move(body));
}

std::unique_ptr<ReturnStatementNode> Parser::parseReturnStatement()
{
    consume(TokenType::Return, "Expected 'return'");
    std::unique_ptr<ExpressionNode> returnValue = nullptr;
    if (!check(TokenType::Semicolon))
    {
        returnValue = parseExpression();
    }
    consume(TokenType::Semicolon, "Expected ';'");
    return std::make_unique<ReturnStatementNode>(currentToken, std::move(returnValue));
}

std::unique_ptr<DeclarationNode> Parser::parseDeclaration()
{
    Token modifier = consume(TokenType::Var, "Expected 'var' lub 'const var'");
    Token name = consume(TokenType::Identifier, "Expected nazwy zmiennej");
    std::unique_ptr<ExpressionNode> initializer = nullptr;
    if (match({TokenType::Assign}))
    {
        initializer = parseExpression();
    }
    consume(TokenType::Semicolon, "Expected ';'");
    return std::make_unique<DeclarationNode>(modifier, name, std::move(initializer));
}

std::unique_ptr<AssignNode> Parser::parseAssign()
{
    Token name = consume(TokenType::Identifier, "Expected variable name");
    consume(TokenType::Assign, "Expected '='");
    std::unique_ptr<ExpressionNode> expr = parseExpression();
    consume(TokenType::Semicolon, "Expected ';'");
    return std::make_unique<AssignNode>(name, currentToken, std::move(expr));
}

std::unique_ptr<ExpressionNode> Parser::parseExpression()
{
    if (check(TokenType::Fun))
    {
        return parseFunctionLiteral();
    }

    std::unique_ptr<ExpressionNode> left = parseSimpleExpression();

    if (match({TokenType::As}))
    {
        Token type = consume(TokenType::Identifier, "Expected a type");
        return std::make_unique<TypeCastNode>(std::move(left), type);
    }

    return left;
}

std::unique_ptr<ExpressionNode> Parser::parseTypeCastExpression()
{
    std::unique_ptr<ExpressionNode> expr = parseLogicalExpr();
    if (match({TokenType::As}))
    {
        Token type = consume(TokenType::Identifier, "Expected a type");
        return std::make_unique<TypeCastNode>(std::move(expr), type);
    }
    return expr;
}

std::unique_ptr<ExpressionNode> Parser::parseLogicalExpr()
{
    std::unique_ptr<ExpressionNode> left = parseRelExpression();
    while (match({TokenType::And, TokenType::Or}))
    {
        Token op = currentToken;
        advance();
        std::unique_ptr<ExpressionNode> right = parseRelExpression();
        left = std::make_unique<BinaryOpNode>(std::move(left), op, std::move(right));
    }
    return left;
}

std::unique_ptr<ExpressionNode> Parser::parseRelExpression()
{
    std::unique_ptr<ExpressionNode> left = parseSimpleExpression();
    while (match({TokenType::Equal, TokenType::NotEqual, TokenType::Greater,
                  TokenType::GreaterEqual, TokenType::Less, TokenType::LessEqual}))
    {
        Token op = advance();
        std::unique_ptr<ExpressionNode> right = parseSimpleExpression();
        left = std::make_unique<BinaryOpNode>(std::move(left), op, std::move(right));
    }
    return left;
}

std::unique_ptr<ExpressionNode> Parser::parseSimpleExpression()
{
    std::unique_ptr<ExpressionNode> left = parseTerm();
    while (match({TokenType::Plus, TokenType::Minus, TokenType::Pipe}))
    {
        Token op = advance();
        std::unique_ptr<ExpressionNode> right = parseTerm();
        left = std::make_unique<BinaryOpNode>(std::move(left), op, std::move(right));
    }
    return left;
}

std::unique_ptr<ExpressionNode> Parser::parseTerm()
{
    std::unique_ptr<ExpressionNode> left = parseFactor();
    while (match({TokenType::Star, TokenType::Slash}))
    {
        Token op = advance();
        std::unique_ptr<ExpressionNode> right = parseFactor();
        left = std::make_unique<BinaryOpNode>(std::move(left), op, std::move(right));
    }
    return left;
}

std::unique_ptr<ExpressionNode> Parser::parseFactor()
{
    std::unique_ptr<ExpressionNode> expr = parseBaseFactor();
    while (check(TokenType::LParen))
    {
        expr = parseFunctionCall(std::move(expr));
    }
    return expr;
}

std::unique_ptr<ExpressionNode> Parser::parseFunctionCall(std::unique_ptr<ExpressionNode> callee)
{
    consume(TokenType::LParen, "Expected '('");
    std::vector<std::unique_ptr<ExpressionNode>> args = parseArgumentList();
    consume(TokenType::RParen, "Expected ')'");
    return std::make_unique<FunctionCallNode>(std::move(callee), std::move(args));
}

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

std::unique_ptr<ExpressionNode> Parser::parseBaseFactor()
{
    if (match({TokenType::Number}))
    {
        return std::make_unique<NumberLiteralNode>(currentToken);
    }
    if (match({TokenType::StringLiteral}))
    {
        return std::make_unique<StringLiteralNode>(currentToken);
    }
    if (match({TokenType::Identifier}))
    {
        return std::make_unique<IdentifierNode>(currentToken);
    }
    if (match({TokenType::LParen}))
    {
        std::unique_ptr<ExpressionNode> expr = parseExpression();
        consume(TokenType::RParen, "Expected ')'");
        return expr;
    }
    if (check(TokenType::Fun))
    {
        return parseFunctionLiteral();
    }
    throw error("Expected an expression");
}

std::unique_ptr<FunctionLiteralNode> Parser::parseFunctionLiteral()
{
    Token funToken = consume(TokenType::Fun, "Expected 'fun'");
    Token lParenToken = consume(TokenType::LParen, "Expected '('");
    std::vector<FuncDefArgument> parameters = parseParameters();
    Token rParenToken = consume(TokenType::RParen, "Expected ')'");
    std::unique_ptr<StatementBlockNode> body = parseStatementBlock();

    return std::make_unique<FunctionLiteralNode>(funToken, lParenToken, parameters, rParenToken,
                                                 std::move(body));
}