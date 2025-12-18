cmake_minimum_required(VERSION 3.10)
project(Translator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(include)

# Source files
set(SOURCES
    src/main.cpp
    src/Lexer.cpp
    src/Parser.cpp
    src/Token.cpp
    src/Variable.cpp
    src/RPNCalculator.cpp
    src/ExpressionTranslator.cpp
)

# Create executable
add_executable(Translator ${SOURCES})

