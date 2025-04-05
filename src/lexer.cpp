#include <cctype>
#include <unordered_map>
#include <cmath>

#include "lexer.hpp"
#include "error_module.hpp"

const std::unordered_map<std::string, TokenType> keywords = {
    {"int", TokenType::Type},      {"float", TokenType::Type},  {"string", TokenType::Type},
    {"var", TokenType::Var},       {"const", TokenType::Const}, {"fun", TokenType::Fun},
    {"return", TokenType::Return}, {"if", TokenType::If},       {"else", TokenType::Else},
    {"while", TokenType::While},   {"as", TokenType::As},       {"print", TokenType::Print}};

Lexer::Lexer(std::istream& inputStream)
    : input(inputStream), line(1), column(0), currentChar()
{
    get();
}

int Lexer::digit_to_int(char digit) const
{
    return static_cast<int>((digit) - ASCII_ZERO);
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
        line++;
        column = 1;
    }
    else
    {
        column++;
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

std::optional<Token> Lexer::tryBuildIdentifier()
{
    if (!std::isalpha(currentChar) && currentChar != '_') return std::nullopt;

    int tokenStartLine = line, tokenStartCol = column;
    std::string ident;
    int currentLen = 0;

    while (std::isalnum(currentChar) || currentChar == '_') {
        ident += currentChar;
        get();
        if (++currentLen >= MAX_IDENTIFIER_LEN)
        {
            Error err = {
                ErrorType::Lexical,
                "Identifier is too long",
                tokenStartLine,
                tokenStartCol
                };
                throw InterpreterException(err);
        }
    }

        if (keywords.count(ident)) {
            return Token(keywords.at(ident), tokenStartLine, tokenStartCol);
    }

    return Token(TokenType::Identifier, ident, tokenStartLine, tokenStartCol);
}

std::optional<Token> Lexer::tryBuildNumber()
{
    if (!std::isdigit(currentChar)) return std::nullopt;
    int tokenStartLine = line, tokenStartCol = column;
 
    int intPart = 0;
    while (std::isdigit(currentChar)) 
    {
        int currentDigit = digit_to_int(currentChar);
        if ((MAXINT - currentDigit)/10 >= intPart)
        {
            intPart *= 10;
            intPart += currentDigit;
        }
        get();
    }
    if (currentChar != '.')
    {
        return Token(TokenType::Number, intPart, tokenStartLine, tokenStartCol);
    }
    get();
    int fracDigits = 0;
    int fracPart = 0;
    while (std::isdigit(currentChar)) {
        fracPart = fracPart * 10 + digit_to_int(currentChar);
        fracDigits++;
        get();
    }
    if (currentChar == '.')
    {
        Error err = {
        ErrorType::Syntax,
        "Invalid float",
        tokenStartLine,
        tokenStartCol
        };
        throw InterpreterException(err);
    }
    float divisor = std::pow(10.0f, fracDigits);
    float finalValue = static_cast<float>(intPart) + (fracPart / divisor);
    return Token(TokenType::Number, finalValue, tokenStartLine, tokenStartCol);

}
std::optional<Token> Lexer::tryBuildString()
{
    if (currentChar != '"')  return std::nullopt;
    int tokenStartLine = line, tokenStartCol = column;
    
    get();
    std::string strLiteral;
    while (currentChar != '"' && currentChar != EOF)
    {
        if (currentChar == '\\')
            {
            get();

            if (currentChar == EOF) 
                break;
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
    if (currentChar == '"') get();
    else if (currentChar == EOF)
    {
        Error err = {
        ErrorType::Lexical,
        "Unterminated string literal",
        tokenStartLine,
        tokenStartCol
        };
    throw InterpreterException(err);        
    }
    return Token(TokenType::StringLiteral, strLiteral, tokenStartLine, tokenStartCol);
}

std::optional<Token> Lexer::tryBuildSymbol()
{
    int tokenStartLine = line, tokenStartCol = column;

    switch (currentChar)
    {
    case '+': return consumeAndReturn(Token(TokenType::Plus, tokenStartLine, tokenStartCol));
    case '-': return consumeAndReturn(Token(TokenType::Minus, tokenStartLine, tokenStartCol));
    case '*': return consumeAndReturn(Token(TokenType::Star, tokenStartLine, tokenStartCol));
    case '/': return consumeAndReturn(Token(TokenType::Slash, tokenStartLine, tokenStartCol));

    case '=':
        get();
        if (currentChar == '=')
            return consumeAndReturn(Token(TokenType::Equal, tokenStartLine, tokenStartCol));
        return Token(TokenType::Assign, tokenStartLine, tokenStartCol);

    case '!':
        get();
        if (currentChar == '=')
            return consumeAndReturn(Token(TokenType::NotEqual, tokenStartLine, tokenStartCol));
        break;

    case '>':
        get();
        if (currentChar == '=')
            return consumeAndReturn(Token(TokenType::GreaterEqual, tokenStartLine, tokenStartCol));
        return Token(TokenType::Greater, tokenStartLine, tokenStartCol);

    case '<':
        get();
        if (currentChar == '=')
            return consumeAndReturn(Token(TokenType::LessEqual, tokenStartLine, tokenStartCol));
        return Token(TokenType::Less, tokenStartLine, tokenStartCol);

    case '|':
        get();
        if (currentChar == '|')
            return consumeAndReturn(Token(TokenType::Or, tokenStartLine, tokenStartCol));
        return Token(TokenType::Pipe, tokenStartLine, tokenStartCol);

    case '@':
        get();
        if (currentChar == '@')
            return consumeAndReturn(Token(TokenType::AtAt, tokenStartLine, tokenStartCol));
        break;

    case '(':
        return consumeAndReturn(Token(TokenType::LParen, tokenStartLine, tokenStartCol));
    case ')':
        return consumeAndReturn(Token(TokenType::RParen, tokenStartLine, tokenStartCol));
    case '[':
        return consumeAndReturn(Token(TokenType::LBracket, tokenStartLine, tokenStartCol));
    case ']':
        return consumeAndReturn(Token(TokenType::RBracket, tokenStartLine, tokenStartCol));
    case ';':
        return consumeAndReturn(Token(TokenType::Semicolon, tokenStartLine, tokenStartCol));
    case ',':
        return consumeAndReturn(Token(TokenType::Comma, tokenStartLine, tokenStartCol));
    case '&':
        get();
        if (currentChar == '&')
            return consumeAndReturn(Token(TokenType::And, tokenStartLine, tokenStartCol));
        break;
    }
    return std::nullopt;
}

Token Lexer::scanToken()
{
    skipWhitespaceAndComments();
    int tokenStartLine = line, tokenStartCol = column; 
    if (currentChar == EOF) return Token(TokenType::EndOfFile, tokenStartLine, tokenStartCol);

    if (auto t = tryBuildIdentifier()) return *t;
    if (auto t = tryBuildNumber())     return *t;
    if (auto t = tryBuildString())     return *t;
    if (auto t = tryBuildSymbol())     return *t;

    char unexpected = currentChar;
    get();
    return Token(TokenType::Unknown, std::string(1, unexpected), tokenStartLine, tokenStartCol);
}
