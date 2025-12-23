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
        
        // Check for unary minus
        if (token.type == TokenType::OPERATOR && token.value == "-") {
            // Unary minus if it's at the start, after '(', after operator, or after comma
            bool isUnary = false;
            if (i == 0) {
                isUnary = true;
            } else {
                TokenType prevType = tokens[i-1].type;
                if (prevType == TokenType::LPAREN || 
                    prevType == TokenType::OPERATOR || 
                    prevType == TokenType::COMMA ||
                    prevType == TokenType::ASSIGN) {
                    isUnary = true;
                }
            }
            
            if (isUnary) {
                // Transform unary minus into: 0 - x
                if (rpnCount < maxRPN) {
                    rpn[rpnCount++] = Token(TokenType::NUMBER, "0", token.position);
                }
                // Continue processing, the minus will be added as binary operator
            }
        }
        
        switch (token.type) {
            case TokenType::NUMBER:
            case TokenType::VARIABLE:
                if (rpnCount < maxRPN) {
                    rpn[rpnCount++] = token;
                }
                break;
                
            case TokenType::FUNCTION:
                // Functions go to stack and will be popped when we reach ')'
                opStack.push(token);
                break;
                
            case TokenType::COMMA:
                // Comma acts as argument separator - pop until we find '(' or function
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
                    // If there was a function before '(', add it to output
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
    // Check for empty expression
    if (count == 0 || (count == 1 && tokens[0].type == TokenType::END)) {
        error = "Empty expression";
        return false;
    }
    
    // Count non-END tokens
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
    
    // Check for multiple assignments
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
    
    // Check token pairs and sequences
    for (int i = 0; i < count - 1; i++) {
        const Token& token = tokens[i];
        const Token& nextToken = tokens[i + 1];
        
        if (token.type == TokenType::END || token.type == TokenType::UNKNOWN) {
            break;
        }
        if (nextToken.type == TokenType::END || nextToken.type == TokenType::UNKNOWN) {
            continue;
        }
        
        // Check for empty parentheses (except after functions)
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
        
        // Check that functions are followed by '('
        if (token.type == TokenType::FUNCTION && nextToken.type != TokenType::LPAREN) {
            error = "Function '" + token.value + "' must be followed by '('";
            return false;
        }
        
        // Check: number/variable followed by number/variable (missing operator)
        if ((token.type == TokenType::NUMBER || token.type == TokenType::VARIABLE) &&
            (nextToken.type == TokenType::NUMBER || nextToken.type == TokenType::VARIABLE)) {
            error = "Missing operator between operands";
            return false;
        }
        
        // Check: number/variable followed by '(' (missing operator, like "5(3)")
        if ((token.type == TokenType::NUMBER || token.type == TokenType::VARIABLE) &&
            nextToken.type == TokenType::LPAREN) {
            error = "Missing operator before '('";
            return false;
        }
        
        // Check: number/variable followed by function (missing operator, like "5sqrt(4)")
        if ((token.type == TokenType::NUMBER || token.type == TokenType::VARIABLE) &&
            nextToken.type == TokenType::FUNCTION) {
            error = "Missing operator before function '" + nextToken.value + "'";
            return false;
        }
        
        // Check: ')' followed by number/variable (missing operator, like "(2)5")
        if (token.type == TokenType::RPAREN &&
            (nextToken.type == TokenType::NUMBER || nextToken.type == TokenType::VARIABLE)) {
            error = "Missing operator after ')'";
            return false;
        }
        
        // Check: ')' followed by '(' (missing operator, like "(2)(3)")
        if (token.type == TokenType::RPAREN && nextToken.type == TokenType::LPAREN) {
            error = "Missing operator between parentheses";
            return false;
        }
        
        // Check: ')' followed by function (missing operator, like "(2)sqrt(4)")
        if (token.type == TokenType::RPAREN && nextToken.type == TokenType::FUNCTION) {
            error = "Missing operator before function '" + nextToken.value + "'";
            return false;
        }
        
        // Check: operator followed by ')' (missing operand)
        if (token.type == TokenType::OPERATOR && token.value != "-" && nextToken.type == TokenType::RPAREN) {
            error = "Missing operand after operator '" + token.value + "'";
            return false;
        }
        
        // Check: comma not in proper context
        if (token.type == TokenType::COMMA) {
            error = "Comma is not supported yet (reserved for future multi-argument functions)";
            return false;
        }
    }
    
    // Check if function is the last token (missing parentheses)
    if (count > 0 && tokens[count - 1].type == TokenType::FUNCTION) {
        error = "Function '" + tokens[count - 1].value + "' must be followed by '('";
        return false;
    }
    
    // Check first token
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
    
    // Check for consecutive operators
    bool lastWasOperator = true; // Start of expression can be unary minus
    bool lastWasValue = false; // Track if last token was a value (number/variable)
    
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
            lastWasOperator = true; // Unary minus allowed after ( or ,
            lastWasValue = false;
        } else if (token.type == TokenType::RPAREN) {
            if (!lastWasValue && i > 0 && tokens[i-1].type != TokenType::RPAREN) {
                // Allow "))" but not "operator)"
                if (tokens[i-1].type == TokenType::OPERATOR || 
                    tokens[i-1].type == TokenType::LPAREN ||
                    tokens[i-1].type == TokenType::COMMA) {
                    error = "Missing operand before ')'";
                    return false;
                }
            }
            lastWasOperator = false;
            lastWasValue = false;
        } else if (token.type == TokenType::NUMBER || token.type == TokenType::VARIABLE) {
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