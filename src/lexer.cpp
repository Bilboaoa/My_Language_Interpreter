#include <cctype>
#include <unordered_map>
#include <cmath>
#include <optional>

#include "lexer.hpp"

namespace
{
const std::unordered_map<std::string, TokenType> keywords = {
    {"int", TokenType::Type},      {"float", TokenType::Type},  {"string", TokenType::Type},
    {"var", TokenType::Var},       {"const", TokenType::Const}, {"fun", TokenType::Fun},
    {"return", TokenType::Return}, {"if", TokenType::If},       {"else", TokenType::Else},
    {"while", TokenType::While},   {"as", TokenType::As}};

int digit_to_int(char digit)
{
    return static_cast<int>((digit) - '0');
}

bool token_known(Token t)
{
    return t.type != TokenType::Unknown;
}

}  // namespace

Lexer::Lexer(std::istream& inputStream) : input(inputStream), currentPosition(), currentChar()
{
    get();
}

void Lexer::throwError(ErrorType type, const std::string& msg, Position pos) const
{
    Error err = {type, msg, pos};
    throw InterpreterException(err);
}

char Lexer::get()
{
    if (input.eof())
    {
        currentChar = EOF;
        return currentChar;
    }
    if (currentChar == '\n')
    {
        currentPosition.line++;
        currentPosition.column = 1;
    }
    else
    {
        currentPosition.column++;
    }
    currentChar = input.get();
    return currentChar;
}

void Lexer::skipWhitespaceAndComments()
{
    while (std::isspace(currentChar) || (currentChar == '/' && input.peek() == '/'))
    {
        if (std::isspace(currentChar))
        {
            get();
        }
        else if (currentChar == '/')
        {
            get();
            get();
            while (currentChar != '\n' && !input.eof()) get();
        }
    }
}

Token Lexer::consumeAndReturn(Token returned)
{
    get();
    return returned;
}

Token Lexer::tryBuildIdentifier()
{
    if (!std::isalpha(currentChar) && currentChar != '_')
        return Token(TokenType::Unknown, currentPosition);

    Position startPos = currentPosition;
    std::string ident;
    int currentLen = 0;

    while (std::isalnum(currentChar) || currentChar == '_')
    {
        ident += currentChar;
        get();
        if (++currentLen >= MAX_IDENTIFIER_LEN)
            throwError(ErrorType::Lexical, "Identifier is too long", startPos);
    }

    auto found = keywords.find(ident);
    if (found != keywords.end())
    {
        if (found->second == TokenType::Type)
            return Token(TokenType::Type, ident, startPos);
        return Token(found->second, startPos);
    }

    return Token(TokenType::Identifier, ident, startPos);
}

Token Lexer::tryBuildNumber()
{
    if (!std::isdigit(currentChar)) return Token(TokenType::Unknown, currentPosition);
    Position startPos = currentPosition;

    int intPart = 0;
    bool firstZero = false;
    if (currentChar == '0')
    {
        get();
        firstZero = true;
    }
    if (!firstZero)
        while (std::isdigit(currentChar))
        {
            int currentDigit = digit_to_int(currentChar);
            if ((MAXINT - currentDigit) / 10 >= intPart)
            {
                intPart *= 10;
                intPart += currentDigit;
            }
            get();
        }
    if (currentChar != '.')
    {
        return Token(TokenType::Number, intPart, startPos);
    }
    get();
    int fracDigits = 0;
    int fracPart = 0;
    while (std::isdigit(currentChar))
    {
        fracPart = fracPart * 10 + digit_to_int(currentChar);
        fracDigits++;
        get();
    }
    if (currentChar == '.') throwError(ErrorType::Syntax, "Invalid float", startPos);

    float divisor = std::pow(10.0f, fracDigits);
    float finalValue = static_cast<float>(intPart) + (fracPart / divisor);
    return Token(TokenType::Number, finalValue, startPos);
}
Token Lexer::tryBuildString()
{
    if (currentChar != '"') return Token(TokenType::Unknown, currentPosition);
    Position startPos = currentPosition;

    get();
    std::string strLiteral;
    while (currentChar != '"' && currentChar != EOF)
    {
        if (currentChar == '\\')
        {
            get();

            if (currentChar == EOF) break;
            switch (currentChar)
            {
                case 'n':
                    strLiteral += '\n';
                    break;
                case 't':
                    strLiteral += '\t';
                    break;
                case '"':
                    strLiteral += '"';
                    break;
                case '\\':
                    strLiteral += '\\';
                    break;
                default:
                    strLiteral += currentChar;
                    break;
            }
            get();
        }
        else
        {
            strLiteral += currentChar;
            get();
        }
    }
    if (currentChar == '"')
        get();
    else if (currentChar == EOF)
        throwError(ErrorType::Lexical, "Unterminated string literal", startPos);
    return Token(TokenType::StringLiteral, strLiteral, startPos);
}

Token Lexer::tryBuildSymbol()
{
    Position startPos = currentPosition;

    switch (currentChar)
    {
        case '+':
            return consumeAndReturn(Token(TokenType::Plus, startPos));
        case '-':
            return consumeAndReturn(Token(TokenType::Minus, startPos));
        case '*':
            return consumeAndReturn(Token(TokenType::Star, startPos));
        case '/':
            return consumeAndReturn(Token(TokenType::Slash, startPos));

        case '=':
            get();
            if (currentChar == '=') return consumeAndReturn(Token(TokenType::Equal, startPos));
            return Token(TokenType::Assign, startPos);

        case '!':
            get();
            if (currentChar == '=') return consumeAndReturn(Token(TokenType::NotEqual, startPos));
            break;

        case '>':
            get();
            if (currentChar == '=')
                return consumeAndReturn(Token(TokenType::GreaterEqual, startPos));
            return Token(TokenType::Greater, startPos);

        case '<':
            get();
            if (currentChar == '=') return consumeAndReturn(Token(TokenType::LessEqual, startPos));
            return Token(TokenType::Less, startPos);

        case '|':
            get();
            if (currentChar == '|') return consumeAndReturn(Token(TokenType::Or, startPos));
            return Token(TokenType::Pipe, startPos);

        case '@':
            get();
            if (currentChar == '@') return consumeAndReturn(Token(TokenType::AtAt, startPos));
            break;

        case '(':
            return consumeAndReturn(Token(TokenType::LParen, startPos));
        case ')':
            return consumeAndReturn(Token(TokenType::RParen, startPos));
        case '[':
            return consumeAndReturn(Token(TokenType::LBracket, startPos));
        case ']':
            return consumeAndReturn(Token(TokenType::RBracket, startPos));
        case ';':
            return consumeAndReturn(Token(TokenType::Semicolon, startPos));
        case ',':
            return consumeAndReturn(Token(TokenType::Comma, startPos));
        case '&':
            get();
            if (currentChar == '&') return consumeAndReturn(Token(TokenType::And, startPos));
            break;
    }
    return Token(TokenType::Unknown, currentPosition);
}

Token Lexer::scanToken()
{
    skipWhitespaceAndComments();
    Position startPos = currentPosition;

    if (currentChar == EOF) return Token(TokenType::EndOfFile, startPos);

    Token token = tryBuildIdentifier();
    if (token_known(token)) return token;
    token = tryBuildNumber();
    if (token_known(token)) return token;
    token = tryBuildString();
    if (token_known(token)) return token;
    token = tryBuildSymbol();
    if (token_known(token)) return token;

    char unexpected = currentChar;
    get();
    return Token(TokenType::Unknown, std::string(1, unexpected), startPos);
}
