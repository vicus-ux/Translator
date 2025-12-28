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
    return op != "^"; 
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
        
        if (token.type == TokenType::OPERATOR && token.value == "-") {
            bool isUnary = false;
            if (i == 0) {
                isUnary = true;
            } else {
                TokenType prevType = tokens[i-1].type;
                if (prevType == TokenType::LPAREN || 
                    prevType == TokenType::OPERATOR || 
                    prevType == TokenType::COMMA ||
                    prevType == TokenType::ASSIGN ||
                    prevType == TokenType::FUNCTION) {
                    isUnary = true;
                }
            }
            
            if (isUnary) {
                // Для унарного минуса добавляем 0 и унарный минус
                // Но не добавляем оператор здесь, он будет обработан потом
                if (rpnCount < maxRPN) {
                    rpn[rpnCount++] = Token(TokenType::NUMBER, "0", token.position);
                    // Унарный минус обрабатываем как обычный оператор
                    // он будет помещен в стек операторов
                }
            }
        }
        
        switch (token.type) {
            case TokenType::NUMBER:
            case TokenType::VARIABLE:
            case TokenType::CONSTANT:
                if (rpnCount < maxRPN) {
                    rpn[rpnCount++] = token;
                }
                break;
                
            case TokenType::FUNCTION:
                opStack.push(token);
                break;
                
            case TokenType::COMMA:
                while (!opStack.empty() && 
                       opStack.top().type != TokenType::LPAREN &&
                       opStack.top().type != TokenType::FUNCTION) {
                    if (rpnCount < maxRPN) {
                        rpn[rpnCount++] = opStack.top();
                    }
                    opStack.pop();
                }
                break;
                
            case TokenType::OPERATOR:
            case TokenType::ASSIGN:
                if (i > 0 && token.type == TokenType::OPERATOR) {
                    const Token& prevToken = tokens[i-1];
                    if (prevToken.type == TokenType::OPERATOR && 
                        prevToken.value != "-" && token.value != "-") {
                        throw std::runtime_error("Two operators in a row: '" + 
                                                prevToken.value + "' and '" + token.value + "'");
                    }
                }
                
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
                    if (!opStack.empty() && opStack.top().type == TokenType::FUNCTION) {
                        if (rpnCount < maxRPN) {
                            rpn[rpnCount++] = opStack.top();
                        }
                        opStack.pop();
                    }
                } else {
                    throw std::runtime_error("Unpaired parenthesis");
                }
                break;
                
            default:
                break;
        }
    }
    
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
    
    if (count == 0 || (count == 1 && tokens[0].type == TokenType::END)) {
        error = "Empty expression";
        return false;
    }
    
    int realCount = 0;
    for (int i = 0; i < count; i++) {
        if (tokens[i].type != TokenType::END && tokens[i].type != TokenType::UNKNOWN) {
            realCount++;
        }
    }
    
    if (realCount == 0) {
        error = "Empty expression";
        return false;
    }
    
    if (!checkParentheses(tokens, count)) {
        error = "Unpaired parentheses";
        return false;
    }
    
    int assignCount = 0;
    for (int i = 0; i < count; i++) {
        if (tokens[i].type == TokenType::ASSIGN) {
            assignCount++;
            if (assignCount > 1) {
                error = "Multiple assignments in one expression are not allowed";
                return false;
            }
        }
    }
    for (int i = 0; i < count - 1; i++) {
        const Token& token = tokens[i];
        const Token& nextToken = tokens[i + 1];
        
        if (token.type == TokenType::END || token.type == TokenType::UNKNOWN) {
            break;
        }
        if (nextToken.type == TokenType::END || nextToken.type == TokenType::UNKNOWN) {
            continue;
        }
        
        if (token.type == TokenType::OPERATOR && token.value == "-" &&
            nextToken.type == TokenType::OPERATOR && nextToken.value == "-") {
            bool isFirstUnary = false;
            if (i == 0) {
                isFirstUnary = true;
            } else {
                TokenType prevType = tokens[i-1].type;
                if (prevType == TokenType::LPAREN || 
                    prevType == TokenType::OPERATOR || 
                    prevType == TokenType::COMMA ||
                    prevType == TokenType::ASSIGN ||
                    prevType == TokenType::FUNCTION) {
                    isFirstUnary = true;
                }
            }
            
            if (isFirstUnary) {
                error = "Multiple unary minus operators are not allowed";
                return false;
            }
        }
        
        if (token.type == TokenType::LPAREN && nextToken.type == TokenType::RPAREN) {
            bool hasFunction = false;
            if (i > 0 && tokens[i - 1].type == TokenType::FUNCTION) {
                hasFunction = true;
            }
            if (!hasFunction) {
                error = "Empty parentheses are not allowed";
                return false;
            }
        }
        
        if (token.type == TokenType::FUNCTION && nextToken.type != TokenType::LPAREN) {
            error = "Function '" + token.value + "' must be followed by '('";
            return false;
        }
        
        if ((token.type == TokenType::NUMBER || token.type == TokenType::VARIABLE || 
             token.type == TokenType::CONSTANT) &&
            (nextToken.type == TokenType::NUMBER || nextToken.type == TokenType::VARIABLE ||
             nextToken.type == TokenType::CONSTANT)) {
            error = "Missing operator between operands";
            return false;
        }
        
        if ((token.type == TokenType::NUMBER || token.type == TokenType::VARIABLE || 
             token.type == TokenType::CONSTANT) &&
            nextToken.type == TokenType::LPAREN) {
            error = "Missing operator before '('";
            return false;
        }
        
        if ((token.type == TokenType::NUMBER || token.type == TokenType::VARIABLE || 
             token.type == TokenType::CONSTANT) &&
            nextToken.type == TokenType::FUNCTION) {
            error = "Missing operator before function '" + nextToken.value + "'";
            return false;
        }
        
        if (token.type == TokenType::RPAREN &&
            (nextToken.type == TokenType::NUMBER || nextToken.type == TokenType::VARIABLE ||
             nextToken.type == TokenType::CONSTANT)) {
            error = "Missing operator after ')'";
            return false;
        }
        
        if (token.type == TokenType::RPAREN && nextToken.type == TokenType::LPAREN) {
            error = "Missing operator between parentheses";
            return false;
        }
        
        if (token.type == TokenType::RPAREN && nextToken.type == TokenType::FUNCTION) {
            error = "Missing operator before function '" + nextToken.value + "'";
            return false;
        }
        
        if (token.type == TokenType::OPERATOR && token.value != "-" && 
            nextToken.type == TokenType::RPAREN) {
            error = "Missing operand after operator '" + token.value + "'";
            return false;
        }
        
        if (token.type == TokenType::COMMA) {
            error = "Comma is not supported yet (reserved for future multi-argument functions)";
            return false;
        }
        
        if (token.type == TokenType::OPERATOR && nextToken.type == TokenType::OPERATOR) {
            if (token.value != "-" && nextToken.value != "-") {
                error = "Two operators in a row: '" + token.value + "' and '" + nextToken.value + "'";
                return false;
            }
        }
    }
    
    if (count > 0 && tokens[count - 1].type == TokenType::FUNCTION) {
        error = "Function '" + tokens[count - 1].value + "' must be followed by '('";
        return false;
    }
    
    if (count > 0) {
        const Token& firstToken = tokens[0];
        if (firstToken.type == TokenType::RPAREN) {
            error = "Expression cannot start with ')'";
            return false;
        }
        if (firstToken.type == TokenType::OPERATOR && firstToken.value != "-") {
            error = "Expression cannot start with operator '" + firstToken.value + "'";
            return false;
        }
        if (firstToken.type == TokenType::COMMA) {
            error = "Expression cannot start with ','";
            return false;
        }
    }
    
    bool lastWasOperator = true; 
    bool lastWasValue = false; 
    
    for (int i = 0; i < count; i++) {
        const Token& token = tokens[i];
        if (token.type == TokenType::END || token.type == TokenType::UNKNOWN) {
            continue;
        }
        
        if (token.isOperator()) {
            if (lastWasOperator && token.value != "-") {
                error = "Two operators in a row";
                return false;
            }
            lastWasOperator = true;
            lastWasValue = false;
        } else if (token.type == TokenType::LPAREN || token.type == TokenType::COMMA) {
            lastWasOperator = true; 
            lastWasValue = false;
        } else if (token.type == TokenType::RPAREN) {
            if (!lastWasValue && i > 0 && tokens[i-1].type != TokenType::RPAREN) {
                if (tokens[i-1].type == TokenType::OPERATOR || 
                    tokens[i-1].type == TokenType::LPAREN ||
                    tokens[i-1].type == TokenType::COMMA) {
                    error = "Missing operand before ')'";
                    return false;
                }
            }
            lastWasOperator = false;
            lastWasValue = false;
        } else if (token.type == TokenType::NUMBER || token.type == TokenType::VARIABLE ||
                   token.type == TokenType::CONSTANT) {
            lastWasOperator = false;
            lastWasValue = true;
        } else if (token.type == TokenType::FUNCTION) {
            lastWasOperator = false;
            lastWasValue = false;
        } else {
            lastWasOperator = false;
            lastWasValue = false;
        }
    }
    
    if (lastWasOperator) {
        error = "Expression ends with operator";
        return false;
    }
    
    return true;
}