#ifndef VARIABLE_H
#define VARIABLE_H

#include <string>

struct Variable {
    std::string name;
    double value;
    
    Variable() : name(""), value(0.0) {}
    Variable(const std::string& n, double v) : name(n), value(v) {}
};

class VariableStorage {
private:
    Variable* variables;
    int capacity;
    int count;
    
    void resize(int newCapacity);
    
public:
    VariableStorage(int initialCapacity = 10);
    ~VariableStorage();
    
    void add(const std::string& name, double value);
    void set(const std::string& name, double value);
    bool get(const std::string& name, double& value) const;
    bool exists(const std::string& name) const;
    void clear();
    
    int size() const { return count; }
    const Variable* getVariables() const { return variables; }
};

#endif // VARIABLE_H

