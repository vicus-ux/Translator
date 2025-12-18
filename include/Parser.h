#ifndef PARSER_H
#define PARSER_H

#include "Token.h"
#include "Stack.h"
#include <string>

class Parser {
private:
    int getPrecedence(const std::string& op) const;
    bool isLeftAssociative(const std::string& op) const;
    bool isValidOperator(const std::string& op) const;
    bool checkParentheses(const Token* tokens, int count) const;
    
public:
    void toRPN(const Token* tokens, int tokenCount, Token* rpn, int& rpnCount, int maxRPN);
    bool validateExpression(const Token* tokens, int count, std::string& error) const;
};

#endif // PARSER_H

