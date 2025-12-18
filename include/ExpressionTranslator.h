#ifndef EXPRESSIONTRANSLATOR_H
#define EXPRESSIONTRANSLATOR_H

#include "Lexer.h"
#include "Parser.h"
#include "RPNCalculator.h"
#include <string>

const int MAX_TOKENS = 1000;
const int MAX_RPN = 1000;

class ExpressionTranslator {
private:
    Lexer lexer;
    Parser parser;
    RPNCalculator calculator;
    
    bool processAssignment(const Token* tokens, int count);
    double requestVariableValue(const std::string& varName);
    
public:
    ExpressionTranslator();
    bool translate(const std::string& expression);
    void showVariables() const;
    void clearVariables();
};

#endif // EXPRESSIONTRANSLATOR_H

