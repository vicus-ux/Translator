#include "ExpressionTranslator.h"
#include <iostream>
#include <string>
#include <limits>
#include <cstring>

void printHelp() {
    std::cout << "\n=== ARITHMETIC EXPRESSION TRANSLATOR ===" << std::endl;
    std::cout << "Supported operations: + - * / ^ (exponentiation)" << std::endl;
    std::cout << "You can use parentheses: (a + b) * c" << std::endl;
    std::cout << "Variable support: x = 5 + 3 * 2" << std::endl;
    std::cout << "\nCommands:" << std::endl;
    std::cout << "  help      - show this help" << std::endl;
    std::cout << "  vars      - show all variables" << std::endl;
    std::cout << "  clear     - clear all variables" << std::endl;
    std::cout << "  exit/quit - exit program" << std::endl;
    std::cout << "========================================" << std::endl;
}

void printWelcome() {
    std::cout << "Welcome to Arithmetic Expression Translator!" << std::endl;
    std::cout << "Type 'help' for commands list." << std::endl;
}

int main() {
    // Remove locale setting or use only for numbers
    // setlocale(LC_ALL, "ru_RU.UTF-8");
    
    ExpressionTranslator translator;
    std::string input;
    
    printWelcome();
    
    while (true) {
        std::cout << "\n>>> ";
        std::getline(std::cin, input);
        
        // Remove leading and trailing whitespace
        const char* whitespace = " \t";
        size_t start = input.find_first_not_of(whitespace);
        if (start == std::string::npos) {
            continue;
        }
        size_t end = input.find_last_not_of(whitespace);
        input = input.substr(start, end - start + 1);
        
        // Check commands
        if (input == "exit" || input == "quit") {
            std::cout << "Exiting program." << std::endl;
            break;
        }
        
        if (input == "help") {
            printHelp();
            continue;
        }
        
        if (input == "vars") {
            translator.showVariables();
            continue;
        }
        
        if (input == "clear") {
            translator.clearVariables();
            continue;
        }
        
        if (input.empty()) {
            continue;
        }
        
        // Process expression
        translator.translate(input);
    }
    
    return 0;
}