#include "ExpressionTranslator.h"
#include <iostream>
#include <iomanip>
#include <limits>
#include <cstring>

ExpressionTranslator::ExpressionTranslator() {}

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
            
            VariableStorage& variables = lexer.getVariables();
            double value = calculator.evaluate(rpn, rpnCount, variables);
            
            lexer.setVariable(varName, value);
            
            std::cout << "Variable '" << varName << "' = " << value << std::endl;
            return true;
        } catch (const std::exception& e) {
            std::cout << "Calculation error: " << e.what() << std::endl;
            return false;
        }
    }
    return false;
}

double ExpressionTranslator::requestVariableValue(const std::string& varName) {
    std::cout << "Enter value for variable '" << varName << "': ";
    double value;
    std::cin >> value;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return value;
}

bool ExpressionTranslator::translate(const std::string& expression) {
    lexer.setInput(expression);
    
    Token tokens[MAX_TOKENS];
    int tokenCount = 0;
    lexer.getAllTokens(tokens, tokenCount, MAX_TOKENS);
    
    for (int i = 0; i < tokenCount; i++) {
        if (tokens[i].type == TokenType::UNKNOWN) {
            std::cout << "Lexical analysis error: unknown symbol '" 
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
        std::cout << "Validation error: " << error << std::endl;
        return false;
    }
    
    try {
        Token rpn[MAX_RPN];
        int rpnCount = 0;
        parser.toRPN(tokens, tokenCount, rpn, rpnCount, MAX_RPN);
        
        RPNCalculator::printRPN(rpn, rpnCount);
        
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
        
        double result = calculator.evaluate(rpn, rpnCount, variables);
        
        std::cout << "\n=== Calculation Result ===" << std::endl;
        std::cout << "Result: " << result << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << std::endl;
        return false;
    }
}

void ExpressionTranslator::showVariables() const {
    const VariableStorage& vars = lexer.getVariables();
    if (vars.size() == 0) {
        std::cout << "No variables defined" << std::endl;
    } else {
        std::cout << "\n=== VARIABLES ===" << std::endl;
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