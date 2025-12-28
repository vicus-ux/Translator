#include "ExpressionTranslator.h"
#include <iostream>
#include <iomanip>
#include <limits>
#include <cstring>

ExpressionTranslator::ExpressionTranslator() {}

static void printTokens(const Token* tokens, int count) {
    std::cout << "\n=== LEXICAL ANALYSIS ===" << std::endl;
    std::cout << "Tokens: ";
    
    for (int i = 0; i < count; i++) {
        if (tokens[i].type == TokenType::END) break;
        
        std::string typeStr;
        switch (tokens[i].type) {
            case TokenType::NUMBER: typeStr = "NUM"; break;
            case TokenType::VARIABLE: typeStr = "VAR"; break;
            case TokenType::CONSTANT: typeStr = "CONST"; break;
            case TokenType::OPERATOR: typeStr = "OP"; break;
            case TokenType::FUNCTION: typeStr = "FUNC"; break;
            case TokenType::LPAREN: typeStr = "("; break;
            case TokenType::RPAREN: typeStr = ")"; break;
            case TokenType::COMMA: typeStr = ","; break;
            case TokenType::ASSIGN: typeStr = "="; break;
            default: typeStr = "UNK"; break;
        }
        
        std::cout << "[" << typeStr << ":" << tokens[i].value << "]";
        if (i < count - 1 && tokens[i+1].type != TokenType::END) {
            std::cout << " ";
        }
    }
    std::cout << std::endl;
}

bool ExpressionTranslator::processAssignment(const Token* tokens, int count) {
    bool foundAssign = false;
    int assignIndex = -1;
    
    for (int i = 0; i < count; i++) {
        if (tokens[i].type == TokenType::ASSIGN) {
            foundAssign = true;
            assignIndex = i;
            break;
        }
    }
    
    if (!foundAssign) {
        return false;
    }
    
    if (count >= 3 && tokens[0].type == TokenType::VARIABLE) {
        std::string varName = tokens[0].value;
        
        int exprCount = count - assignIndex - 1;
        Token exprTokens[MAX_TOKENS];
        
        for (int i = 0; i < exprCount; i++) {
            exprTokens[i] = tokens[assignIndex + 1 + i];
        }
        
        if (exprCount > 0 && exprTokens[exprCount - 1].type == TokenType::END) {
            exprCount--;
        }
        
        try {
            std::string error;
            if (!parser.validateExpression(exprTokens, exprCount, error)) {
                std::cout << "Validation error: " << error << std::endl;
                return false;
            }
            
            Token rpn[MAX_RPN];
            int rpnCount = 0;
            parser.toRPN(exprTokens, exprCount, rpn, rpnCount, MAX_RPN);
            
            std::cout << "\nAssignment: " << varName << " = ";
            for (int i = 0; i < exprCount; i++) {
                std::cout << exprTokens[i].value;
            }
            std::cout << std::endl;
            
            printTokens(exprTokens, exprCount);
            
            std::cout << "Polish notation: ";
            for (int i = 0; i < rpnCount; i++) {
                std::cout << rpn[i].value << " ";
            }
            std::cout << std::endl;
            
            VariableStorage& variables = lexer.getVariables();
            double value = calculator.evaluate(rpn, rpnCount, variables, lexer);
            
            lexer.setVariable(varName, value);
            
            std::cout << "Result: " << varName << " = " << value << std::endl;
            return true;
            
        } catch (const std::exception& e) {
            std::cout << "Calculation error: " << e.what() << std::endl;
            return false;
        }
    }
    return false;
}

double ExpressionTranslator::requestVariableValue(const std::string& varName) {
    std::cout << "Enter value for '" << varName << "': ";
    double value;
    std::cin >> value;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return value;
}

bool ExpressionTranslator::translate(const std::string& expression) {
    std::cout << "\nExpression: " << expression << std::endl;
    
    lexer.setInput(expression);
    
    Token tokens[MAX_TOKENS];
    int tokenCount = 0;
    lexer.getAllTokens(tokens, tokenCount, MAX_TOKENS);
    
    for (int i = 0; i < tokenCount; i++) {
        if (tokens[i].type == TokenType::UNKNOWN) {
            std::cout << "Lexical error: unknown symbol '" 
                      << tokens[i].value << "' at position " << tokens[i].position << std::endl;
            return false;
        }
    }
    
    printTokens(tokens, tokenCount);
    
    if (processAssignment(tokens, tokenCount)) {
        return true;
    }
    
    std::string error;
    if (!parser.validateExpression(tokens, tokenCount, error)) {
        std::cout << "Syntax error: " << error << std::endl;
        return false;
    }
    
    try {
        Token rpn[MAX_RPN];
        int rpnCount = 0;
        parser.toRPN(tokens, tokenCount, rpn, rpnCount, MAX_RPN);
        
        std::cout << "Polish notation: ";
        for (int i = 0; i < rpnCount; i++) {
            std::cout << rpn[i].value << " ";
        }
        std::cout << std::endl;
        
        VariableStorage& variables = lexer.getVariables();
        for (int i = 0; i < tokenCount; i++) {
            if (tokens[i].type == TokenType::VARIABLE) {
                double value;
                if (!variables.get(tokens[i].value, value)) {
                    value = requestVariableValue(tokens[i].value);
                    lexer.setVariable(tokens[i].value, value);
                }
            }
        }
        
        double result = calculator.evaluate(rpn, rpnCount, variables, lexer);
        
        std::cout << "Result: " << std::fixed << std::setprecision(6) << result << std::endl;
        
        return true;
        
    } catch (const std::exception& e) {
        std::cout << "Calculation error: " << e.what() << std::endl;
        return false;
    }
}

void ExpressionTranslator::showVariables() const {
    const VariableStorage& vars = lexer.getVariables();
    if (vars.size() == 0) {
        std::cout << "No variables defined" << std::endl;
    } else {
        std::cout << "\nVariables:" << std::endl;
        const Variable* variables = vars.getVariables();
        for (int i = 0; i < vars.size(); i++) {
            std::cout << "  " << variables[i].name << " = " << variables[i].value << std::endl;
        }
    }
}

void ExpressionTranslator::clearVariables() {
    lexer.getVariables().clear();
    std::cout << "All variables cleared" << std::endl;
}