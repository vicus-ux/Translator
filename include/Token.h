#ifndef TOKEN_H
#define TOKEN_H

#include <string>

enum class TokenType {
    NUMBER,
    VARIABLE,
    CONSTANT,
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
    
    Token(TokenType t = TokenType::END, const std::string& v = "", int p = 0)
        : type(t), value(v), position(p) {}
    
    bool isOperator() const {
        return type == TokenType::OPERATOR || type == TokenType::ASSIGN;
    }
};

#endif // TOKEN_H