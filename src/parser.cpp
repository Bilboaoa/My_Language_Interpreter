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

bool Parser::isIn(std::vector<TokenType> types) const
{
    for (TokenType type : types)
    {
        if (check(type)) return true;
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

// Program         = { FunctionDeclaration | Declaration };
std::unique_ptr<ProgramNode> Parser::parseProgram()
{
    std::vector<std::unique_ptr<AstNode>> declarations;
    while (!check(TokenType::EndOfFile))
    {
        if (auto funcDeclaration = parseFunctionDeclaration())
        {
            declarations.push_back(std::move(funcDeclaration));
        }
        else if (auto declaration = parseDeclaration())
        {
            consume(TokenType::Semicolon, "Expected ';' after declaration");
            declarations.push_back(std::move(declaration));
        }
        else
            throw error("Unexpected token in between declarations");
    }
    return std::make_unique<ProgramNode>(std::move(declarations));
}

// FunctionDeclaration = “fun”, id, “(“, [ Parameters ], “)”, StatementBlock ;
std::unique_ptr<FunctionDeclarationNode> Parser::parseFunctionDeclaration()
{
    if (!check(TokenType::Fun)) return nullptr;
    consume(TokenType::Fun, "Expected 'fun'");
    Token name = consume(TokenType::Identifier, "Expected function's name");
    consume(TokenType::LParen, "Expected '('");
    std::vector<FuncDefArgument> params = parseParameters();
    consume(TokenType::RParen, "Expected ')'");
    std::unique_ptr<StatementBlockNode> body = parseStatementBlock();

    return std::make_unique<FunctionDeclarationNode>(name, params, std::move(body));
}

// Parameters = Parameter, {“,”, Parameter }
// Parameter       = (“const” | “var”), id ;
std::vector<FuncDefArgument> Parser::parseParameters()
{
    std::vector<FuncDefArgument> params;
    if (!check(TokenType::RParen))
    {
        do
        {
            Token tmp(TokenType::Unknown, currentToken.startPosition);
            if (check(TokenType::Const))
            {
                tmp = consume(TokenType::Const, "Expected 'const'");
            }
            else if (check(TokenType::Var))
            {
                tmp = consume(TokenType::Var, "Expected 'var'");
            }
            else
                throw error("Expected 'const' or 'var'");
            Token tmpId = consume(TokenType::Identifier, "Expected param's name");
            params.push_back(FuncDefArgument{tmp, tmpId});
        } while (match({TokenType::Comma}));
    }
    return params;
}

// StatementBlock = “[“, { Statement }, “]” ;
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
    if (!check(TokenType::If)) return nullptr;
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

// WhileStatement = “while”, “(“, LogicalExpr, “)”, StatementBlock ;
std::unique_ptr<WhileStatementNode> Parser::parseWhileStatement()
{
    if (!check(TokenType::While)) return nullptr;
    consume(TokenType::While, "Expected 'while'");
    consume(TokenType::LParen, "Expected '('");
    std::unique_ptr<ExpressionNode> condition = parseLogicalExpr();
    consume(TokenType::RParen, "Expected ')'");
    std::unique_ptr<StatementBlockNode> body = parseStatementBlock();
    return std::make_unique<WhileStatementNode>(currentToken, currentToken, std::move(condition),
                                                currentToken, std::move(body));
}

// ReturnStatement = “return”, [ Expression ];
std::unique_ptr<ReturnStatementNode> Parser::parseReturnStatement()
{
    if (!check(TokenType::Return)) return nullptr;
    consume(TokenType::Return, "Expected 'return'");
    std::unique_ptr<ExpressionNode> returnValue = nullptr;
    if (!check(TokenType::Semicolon))
    {
        returnValue = parseExpression();
    }
    return std::make_unique<ReturnStatementNode>(currentToken, std::move(returnValue));
}

// Declaration = (“var” | “const var”), id, [“=”, Expression] ;
std::unique_ptr<DeclarationNode> Parser::parseDeclaration()
{
    if (!check(TokenType::Const) && !check(TokenType::Var)) return nullptr;
    Token modifier = Token(TokenType::Unknown, currentToken.startPosition);
    if (check(TokenType::Const))
        modifier = consume(TokenType::Const, "Expected 'const'");
    else
        modifier = consume(TokenType::Var, "Expected 'var'");
    Token name = consume(TokenType::Identifier, "Expected variable's name");
    std::unique_ptr<ExpressionNode> initializer = nullptr;
    if (match({TokenType::Assign}))
    {
        initializer = parseExpression();
    }
    return std::make_unique<DeclarationNode>(modifier, name, std::move(initializer));
}

// IdOrCallAssign = id, PossibleAssignOrCall ;
std::unique_ptr<StatementNode> Parser::parseIdOrCallAssign()
{
    if (!check(TokenType::Identifier)) return nullptr;
    Token idToken = consume(TokenType::Identifier, "Expected identifier");
    return parsePossibleAssignOrCall(idToken);
}

// PossibleAssignOrCall = "=" Expression ";" | [ CallArguments ] ";" ;
std::unique_ptr<StatementNode> Parser::parsePossibleAssignOrCall(Token idToken)
{
    if (match({TokenType::Assign}))
    {
        std::unique_ptr<ExpressionNode> expr = parseExpression();
        std::unique_ptr<AssignNode> assigned =
            std::make_unique<AssignNode>(idToken, currentToken, std::move(expr));
        consume(TokenType::Semicolon, "No semicolan after assign");
        return assigned;
    }
    else if (check(TokenType::LParen))
    {
        std::unique_ptr<ExpressionNode> callee = std::make_unique<IdentifierNode>(idToken);
        std::unique_ptr<ExpressionNode> call = parseFunctionCall(std::move(callee));
        std::unique_ptr<ExpressionStatementNode> node =
            std::make_unique<ExpressionStatementNode>(std::move(call));
        consume(TokenType::Semicolon, "No semicolan after call");
        return node;
    }
    return nullptr;
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

// Expression = TypeCastExpression | FunctionLiteral;
std::unique_ptr<ExpressionNode> Parser::parseExpression()
{
    if (check(TokenType::Fun))
    {
        return parseFunctionLiteral();
    }

    std::unique_ptr<ExpressionNode> left = parseSimpleExpression();

    if (check(TokenType::As))
    {
        return parseTypeCastExpression(std::move(left));
    }

    return left;
}

// TypeCastExpression = Expression, “as”, Type ;
std::unique_ptr<ExpressionNode> Parser::parseTypeCastExpression(
    std::unique_ptr<ExpressionNode> expr)
{
    if (match({TokenType::As}))
    {
        Token type = consume(TokenType::Type, "Expected a type");
        return std::make_unique<TypeCastNode>(std::move(expr), type);
    }
    return expr;
}

// LogicalExpr   = RelExpression, { LogicalExpr, RelExpression } ;
std::unique_ptr<ExpressionNode> Parser::parseLogicalExpr()
{
    std::unique_ptr<ExpressionNode> left = parseRelExpression();
    while (isIn({TokenType::And, TokenType::Or}))
    {
        Token op = advance();
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
        Token op = advance();
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
        Token op = advance();
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
        Token op = advance();
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
        Token number = consume(TokenType::Number, "Expected a number");
        return std::make_unique<NumberLiteralNode>(number);
    }
    if (check(TokenType::StringLiteral))
    {
        Token str = consume(TokenType::StringLiteral, "Expected string literal");
        return std::make_unique<StringLiteralNode>(str);
    }
    if (check(TokenType::Identifier))
    {
        Token id = consume(TokenType::Identifier, "Expected an identification");
        return std::make_unique<IdentifierNode>(id);
    }
    if (check(TokenType::LParen))
    {
        consume(TokenType::RParen, "Expected '('");
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

// FunctionLiteral = "fun", "(", [ Parameters ], ")", StatementBlock ;
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