#ifndef RPNCALCULATOR_H
#define RPNCALCULATOR_H

#include "Token.h"
#include "Variable.h"

class RPNCalculator {
public:
    double performOperation(double a, double b, const std::string& op) const;
    double evaluate(const Token* rpn, int rpnCount, VariableStorage& variables);
    static void printRPN(const Token* rpn, int count);
};

#endif // RPNCALCULATOR_H

