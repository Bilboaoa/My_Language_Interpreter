#include <cctype>
#include <unordered_map>
#include <cmath>

#include "lexer.hpp"

const std::unordered_map<std::string, TokenType> keywords = {
    {"int", TokenType::Type},      {"float", TokenType::Type},  {"string", TokenType::Type},
    {"var", TokenType::Var},       {"const", TokenType::Const}, {"fun", TokenType::Fun},
    {"return", TokenType::Return}, {"if", TokenType::If},       {"else", TokenType::Else},
    {"while", TokenType::While},   {"as", TokenType::As},       {"print", TokenType::Print}};

Lexer::Lexer(CharStream& inputStream)
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

Token Lexer::scanToken()
{
    skipWhitespaceAndComments();
    int tokenStartLine = line, tokenStartCol = column; 
    if (input.eof()) return Token(TokenType::EndOfFile, "", tokenStartLine, tokenStartCol);

    if (std::isalpha(currentChar) || currentChar == '_')
    {
        std::string ident;
        while (std::isalnum(currentChar) || currentChar == '_')
        {
            ident += currentChar;
            get();
        }
        if (keywords.count(ident))
        {
            return Token(keywords.at(ident), ident, tokenStartLine, tokenStartCol);
        }
        return Token(TokenType::Identifier, ident, tokenStartLine, tokenStartCol);
    }

    if (std::isdigit(currentChar))
    {
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

        float divisor = std::pow(10.0f, fracDigits);
        float finalValue = static_cast<float>(intPart) + (fracPart / divisor);
        return Token(TokenType::Number, finalValue, tokenStartLine, tokenStartCol);
    }

    if (currentChar == '"')
    {
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
        return Token(TokenType::StringLiteral, strLiteral, tokenStartLine, tokenStartCol);
    }

    switch (currentChar)
    {
        case '+':
            get();
            return Token(TokenType::Plus, tokenStartLine, tokenStartCol);
        case '-':
            get();
            return Token(TokenType::Minus, tokenStartLine, tokenStartCol);
        case '*':
            get();
            return Token(TokenType::Star, tokenStartLine, tokenStartCol);
        case '/':
            get();
            return Token(TokenType::Slash, tokenStartLine, tokenStartCol);
        case '=':
            get();
            if (currentChar == '=')
            {
                get();
                return Token(TokenType::EqualEqual, tokenStartLine, tokenStartCol);
            }
            return Token(TokenType::Assign, tokenStartLine, tokenStartCol);
        case '!':
            get();
            if (currentChar == '=')
            {
                get();
                return Token(TokenType::NotEqual, tokenStartLine, tokenStartCol);
            }
            break;
        case '>':
            get();
            if (currentChar == '=')
            {
                get();
                return Token(TokenType::GreaterEqual, tokenStartLine, tokenStartCol);
            }
            return Token(TokenType::Greater, tokenStartLine, tokenStartCol);
        case '<':
            get();
            if (currentChar == '=')
            {
                get();
                return Token(TokenType::LessEqual, tokenStartLine, tokenStartCol);
            }
            return Token(TokenType::Less, tokenStartLine, tokenStartCol);
        case '|':
            get();
            if (currentChar == '|')
            {
                get();
                return Token(TokenType::Or, tokenStartLine, tokenStartCol);
            }
            return Token(TokenType::Pipe, tokenStartLine, tokenStartCol);
        case '@':
            get();
            if (currentChar == '@')
            {
                get();
                return Token(TokenType::AtAt, tokenStartLine, tokenStartCol);
            }
            break;
        case '(':
            get();
            return Token(TokenType::LParen, tokenStartLine, tokenStartCol);
        case ')':
            get();
            return Token(TokenType::RParen, tokenStartLine, tokenStartCol);
        case '[':
            get();
            return Token(TokenType::LBracket, tokenStartLine, tokenStartCol);
        case ']':
            get();
            return Token(TokenType::RBracket, tokenStartLine, tokenStartCol);
        case ';':
            get();
            return Token(TokenType::Semicolon, tokenStartLine, tokenStartCol);
        case ',':
            get();
            return Token(TokenType::Comma, tokenStartLine, tokenStartCol);
        case '&':
            get();
            if (currentChar == '&')
            {
                get();
                return Token(TokenType::And, tokenStartLine, tokenStartCol);
            }
            break;
    }
    char unexpected = currentChar;
    get();
    return Token(TokenType::Unknown, std::string(1, unexpected), tokenStartLine, tokenStartCol);
}

std::vector<Token> Lexer::tokenize()
{
    std::vector<Token> tokens;
    Token token = scanToken();

    while (token.type != TokenType::EndOfFile)
    {
        tokens.push_back(token);
        token = scanToken();
    }

    tokens.push_back(token);
    return tokens;
}
