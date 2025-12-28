#include "Token.h"
#include <iostream>

std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::NUMBER: return "NUMBER";
        case TokenType::VARIABLE: return "VARIABLE";
        case TokenType::OPERATOR: return "OPERATOR";
        case TokenType::FUNCTION: return "FUNCTION";
        case TokenType::LPAREN: return "LPAREN";
        case TokenType::RPAREN: return "RPAREN";
        case TokenType::COMMA: return "COMMA";
        case TokenType::ASSIGN: return "ASSIGN";
        case TokenType::END: return "END";
        case TokenType::UNKNOWN: return "UNKNOWN";
        default: return "UNKNOWN";
    }
}

void printTokens(const Token* tokens, int count) {
    std::cout << "\n=== Lexical Analysis ===" << std::endl;
    std::cout << "Tokens:" << std::endl;
    
    for (int i = 0; i < count; i++) {
        const Token& token = tokens[i];
        if (token.type == TokenType::END) break;
        if (token.type == TokenType::UNKNOWN) {
            std::cout << "Error: unknown symbol '" << token.value 
                      << "' at position " << token.position << std::endl;
            break;
        }
        
        std::cout << "  " << tokenTypeToString(token.type) 
                  << " '" << token.value << "'"
                  << " (position: " << token.position << ")" << std::endl;
    }
}