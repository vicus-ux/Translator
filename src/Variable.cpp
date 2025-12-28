#include "Variable.h"
#include <cstring>
#include <stdexcept>

VariableStorage::VariableStorage(int initialCapacity) 
    : capacity(initialCapacity), count(0) {
    variables = new Variable[capacity];
}

VariableStorage::~VariableStorage() {
    delete[] variables;
}

void VariableStorage::resize(int newCapacity) {
    Variable* newVariables = new Variable[newCapacity];
    for (int i = 0; i < count; i++) {
        newVariables[i] = variables[i];
    }
    delete[] variables;
    variables = newVariables;
    capacity = newCapacity;
}

void VariableStorage::add(const std::string& name, double value) {

    for (int i = 0; i < count; i++) {
        if (variables[i].name == name) {
            variables[i].value = value;
            return;
        }
    }
    if (count >= capacity) {
        resize(capacity * 2);
    }
    
    variables[count].name = name;
    variables[count].value = value;
    count++;
}

void VariableStorage::set(const std::string& name, double value) {
    for (int i = 0; i < count; i++) {
        if (variables[i].name == name) {
            variables[i].value = value;
            return;
        }
    }
    add(name, value);
}

bool VariableStorage::get(const std::string& name, double& value) const {
    for (int i = 0; i < count; i++) {
        if (variables[i].name == name) {
            value = variables[i].value;
            return true;
        }
    }
    return false;
}

bool VariableStorage::exists(const std::string& name) const {
    for (int i = 0; i < count; i++) {
        if (variables[i].name == name) {
            return true;
        }
    }
    return false;
}

void VariableStorage::clear() {
    count = 0;
}