#include "RPNCalculator.h"
#include "Stack.h"
#include <iostream>
#include <cmath>
#include <sstream>
#include <cstdlib>

double RPNCalculator::performOperation(double a, double b, const std::string& op) const {
    if (op == "+") {
        return a + b;
    } else if (op == "-") {
        return a - b;
    } else if (op == "*") {
        return a * b;
    } else if (op == "/") {
        if (b == 0) {
            throw std::runtime_error("Division by zero");
        }
        return a / b;
    } else if (op == "^") {
        return pow(a, b);
    } else {
        throw std::runtime_error("Unknown operator: " + op);
    }
}

double RPNCalculator::evaluate(const Token* rpn, int rpnCount, VariableStorage& variables) {
    Stack<double> values;
    
    for (int i = 0; i < rpnCount; i++) {
        const Token& token = rpn[i];
        
        if (token.type == TokenType::NUMBER) {
            values.push(std::atof(token.value.c_str()));
        } 
        else if (token.type == TokenType::VARIABLE) {
            double value;
            if (variables.get(token.value, value)) {
                values.push(value);
            } else {
                std::ostringstream error;
                error << "Undefined variable: " << token.value;
                throw std::runtime_error(error.str());
            }
        }
        else if (token.type == TokenType::FUNCTION) {
            if (values.size() < 1) {
                throw std::runtime_error("Not enough arguments for function " + token.value);
            }
            
            double arg = values.top(); values.pop();
            double result;
            
            if (token.value == "sqrt") {
                if (arg < 0) {
                    throw std::runtime_error("sqrt: argument must be non-negative");
                }
                result = sqrt(arg);
            } 
            else if (token.value == "sin") {
                result = sin(arg);
            } 
            else if (token.value == "cos") {
                result = cos(arg);
            }
            else {
                throw std::runtime_error("Unknown function: " + token.value);
            }
            
            values.push(result);
        }
        else if (token.type == TokenType::OPERATOR) {
            if (values.size() < 2) {
                throw std::runtime_error("Not enough operands for operator " + token.value);
            }
            
            double b = values.top(); values.pop();
            double a = values.top(); values.pop();
            double result = performOperation(a, b, token.value);
            values.push(result);
        }
        else if (token.type == TokenType::ASSIGN) {
            // Handle assignment in RPN
            if (values.size() < 2) {
                throw std::runtime_error("Not enough operands for assignment");
            }
            
            double value = values.top(); values.pop();
            values.push(value);
        }
    }
    
    if (values.size() != 1) {
        throw std::runtime_error("Invalid expression");
    }
    
    return values.top();
}

void RPNCalculator::printRPN(const Token* rpn, int count) {
    std::cout << "\n=== Reverse Polish Notation (RPN) ===" << std::endl;
    std::cout << "Expression: ";
    
    for (int i = 0; i < count; i++) {
        std::cout << rpn[i].value;
        if (i < count - 1) {
            std::cout << " ";
        }
    }
    std::cout << std::endl;
}