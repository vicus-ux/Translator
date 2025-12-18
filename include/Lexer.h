#ifndef LEXER_H
#define LEXER_H

#include "Token.h"
#include "Variable.h"
#include <string>

class Lexer {
public:
    enum class State {
        START,
        IN_NUMBER,
        IN_VARIABLE,
        IN_OPERATOR,
        DONE
    };
    
private:
    std::string input;
    int pos;
    VariableStorage variables;
    
    void skipWhitespace();
    bool isOperator(char c) const;
    char peek() const;
    char advance();
    
public:
    Lexer(const std::string& str = "");
    void setInput(const std::string& str);
    Token getNextToken();
    void getAllTokens(Token* tokens, int& count, int maxTokens);
    
    VariableStorage& getVariables();
    const VariableStorage& getVariables() const;
    void setVariable(const std::string& name, double value);
    bool getVariable(const std::string& name, double& value) const;
    bool hasVariable(const std::string& name) const;
};

#endif // LEXER_H

