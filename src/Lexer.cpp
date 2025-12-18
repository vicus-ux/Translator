#include "Lexer.h"
#include <cctype>

void Lexer::skipWhitespace() {
    while (pos < input.length() && std::isspace(input[pos])) {
        pos++;
    }
}

bool Lexer::isOperator(char c) const {
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '^' || c == '=';
}

char Lexer::peek() const {
    if (pos < input.length()) {
        return input[pos];
    }
    return '\0';
}

char Lexer::advance() {
    if (pos < input.length()) {
        return input[pos++];
    }
    return '\0';
}

Lexer::Lexer(const std::string& str) : input(str), pos(0) {}

void Lexer::setInput(const std::string& str) {
    input = str;
    pos = 0;
}

Token Lexer::getNextToken() {
    skipWhitespace();
    
    if (pos >= input.length()) {
        return Token(TokenType::END, "", pos);
    }
    
    State state = State::START;
    State prevState = State::START;
    std::string tokenValue;
    int startPos = pos;
    
    while (true) {
        char currentChar = peek();
        
        switch (state) {
            case State::START:
                if (std::isdigit(currentChar) || currentChar == '.') {
                    state = State::IN_NUMBER;
                    tokenValue += advance();
                } else if (std::isalpha(currentChar) || currentChar == '_') {
                    state = State::IN_VARIABLE;
                    tokenValue += advance();
                } else if (isOperator(currentChar)) {
                    tokenValue += advance();
                    if (tokenValue == "=") {
                        return Token(TokenType::ASSIGN, tokenValue, startPos);
                    } else {
                        return Token(TokenType::OPERATOR, tokenValue, startPos);
                    }
                } else if (currentChar == '(') {
                    advance();
                    return Token(TokenType::LPAREN, "(", startPos);
                } else if (currentChar == ')') {
                    advance();
                    return Token(TokenType::RPAREN, ")", startPos);
                } else {
                    std::string unknown(1, advance());
                    return Token(TokenType::UNKNOWN, unknown, startPos);
                }
                break;
                
            case State::IN_NUMBER:
                if (std::isdigit(currentChar) || currentChar == '.') {
                    tokenValue += advance();
                } else {
                    return Token(TokenType::NUMBER, tokenValue, startPos);
                }
                break;
                
            case State::IN_VARIABLE:
                if (std::isalnum(currentChar) || currentChar == '_') {
                    tokenValue += advance();
                } else {
                    return Token(TokenType::VARIABLE, tokenValue, startPos);
                }
                break;
                
            case State::DONE:
                break;
        }
        
        if (state == State::DONE) {
            break;
        }
    }
    
    return Token(TokenType::UNKNOWN, "", startPos);
}

void Lexer::getAllTokens(Token* tokens, int& count, int maxTokens) {
    count = 0;
    Token token;
    
    do {
        token = getNextToken();
        if (count < maxTokens) {
            tokens[count++] = token;
        } else {
            break;
        }
    } while (token.type != TokenType::END && token.type != TokenType::UNKNOWN);
}

VariableStorage& Lexer::getVariables() {
    return variables;
}

const VariableStorage& Lexer::getVariables() const {
    return variables;
}

void Lexer::setVariable(const std::string& name, double value) {
    variables.set(name, value);
}

bool Lexer::getVariable(const std::string& name, double& value) const {
    return variables.get(name, value);
}

bool Lexer::hasVariable(const std::string& name) const {
    return variables.exists(name);
}