#ifndef TOKEN_H
#define TOKEN_H

#include <string>

enum class TokenType {
    NUMBER,
    VARIABLE,
    OPERATOR,
    FUNCTION,
    LPAREN,
    RPAREN,
    COMMA,
    ASSIGN,
    END,
    UNKNOWN
};

struct Token {
    TokenType type;
    std::string value;
    int position;
    
    Token() : type(TokenType::UNKNOWN), value(""), position(0) {}
    Token(TokenType t, const std::string& v, int p) : type(t), value(v), position(p) {}
    
    bool isOperator() const {
        return type == TokenType::OPERATOR;
    }
    
    bool isFunction() const {
        return type == TokenType::FUNCTION;
    }
};

std::string tokenTypeToString(TokenType type);
void printTokens(const Token* tokens, int count);

#endif // TOKEN_H

