#include "Parser.h"
#include <cmath>
#include <cstring>

int Parser::getPrecedence(const std::string& op) const {
    if (op == "^") return 4;
    if (op == "*" || op == "/") return 3;
    if (op == "+" || op == "-") return 2;
    if (op == "=") return 1;
    return 0;
}

bool Parser::isLeftAssociative(const std::string& op) const {
    return op != "^"; // Exponentiation is right-associative
}

bool Parser::isValidOperator(const std::string& op) const {
    return op == "+" || op == "-" || op == "*" || 
           op == "/" || op == "^" || op == "=";
}

void Parser::toRPN(const Token* tokens, int tokenCount, Token* rpn, int& rpnCount, int maxRPN) {
    Stack<Token> opStack;
    rpnCount = 0;
    
    for (int i = 0; i < tokenCount; i++) {
        const Token& token = tokens[i];
        
        if (token.type == TokenType::END || token.type == TokenType::UNKNOWN) {
            break;
        }
        
        switch (token.type) {
            case TokenType::NUMBER:
            case TokenType::VARIABLE:
                if (rpnCount < maxRPN) {
                    rpn[rpnCount++] = token;
                }
                break;
                
            case TokenType::OPERATOR:
            case TokenType::ASSIGN:
                while (!opStack.empty() &&
                       opStack.top().type != TokenType::LPAREN &&
                       ((getPrecedence(opStack.top().value) > getPrecedence(token.value)) ||
                        (getPrecedence(opStack.top().value) == getPrecedence(token.value) && 
                         isLeftAssociative(token.value)))) {
                    if (rpnCount < maxRPN) {
                        rpn[rpnCount++] = opStack.top();
                    }
                    opStack.pop();
                }
                opStack.push(token);
                break;
                
            case TokenType::LPAREN:
                opStack.push(token);
                break;
                
            case TokenType::RPAREN:
                while (!opStack.empty() && opStack.top().type != TokenType::LPAREN) {
                    if (rpnCount < maxRPN) {
                        rpn[rpnCount++] = opStack.top();
                    }
                    opStack.pop();
                }
                if (!opStack.empty() && opStack.top().type == TokenType::LPAREN) {
                    opStack.pop();
                } else {
                    throw std::runtime_error("Unpaired parenthesis");
                }
                break;
                
            default:
                break;
        }
    }
    
    // Pop remaining operators from stack
    while (!opStack.empty()) {
        if (opStack.top().type == TokenType::LPAREN) {
            throw std::runtime_error("Unpaired parenthesis");
        }
        if (rpnCount < maxRPN) {
            rpn[rpnCount++] = opStack.top();
        }
        opStack.pop();
    }
}

bool Parser::checkParentheses(const Token* tokens, int count) const {
    int balance = 0;
    
    for (int i = 0; i < count; i++) {
        const Token& token = tokens[i];
        if (token.type == TokenType::LPAREN) {
            balance++;
        } else if (token.type == TokenType::RPAREN) {
            balance--;
            if (balance < 0) {
                return false;
            }
        }
    }
    
    return balance == 0;
}

bool Parser::validateExpression(const Token* tokens, int count, std::string& error) const {
    if (!checkParentheses(tokens, count)) {
        error = "Unpaired parentheses";
        return false;
    }
    
    // Check for consecutive operators
    bool lastWasOperator = true; // Start of expression can be unary minus
    
    for (int i = 0; i < count; i++) {
        const Token& token = tokens[i];
        if (token.type == TokenType::END || token.type == TokenType::UNKNOWN) {
            continue;
        }
        
        if (token.isOperator()) {
            if (lastWasOperator && token.value != "-") { // Unary minus allowed
                error = "Two operators in a row: " + token.value;
                return false;
            }
            lastWasOperator = true;
        } else if (token.type == TokenType::LPAREN || token.type == TokenType::RPAREN) {
            // Parentheses reset the flag
            lastWasOperator = (token.type == TokenType::LPAREN);
        } else {
            lastWasOperator = false;
        }
    }
    
    if (lastWasOperator) {
        error = "Expression ends with operator";
        return false;
    }
    
    return true;
}