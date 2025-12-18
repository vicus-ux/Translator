#ifndef STACK_H
#define STACK_H

#include <vector>
#include <stdexcept>

template<class T>
class Stack
{
    std::vector<T> data;
public:
    Stack() = default;
    
    void push(const T& el)
    {
        data.push_back(el);
    }
    
    void pop()
    {
        if (empty())
        {
            throw std::out_of_range("Stack is empty");
        }
        data.pop_back();
    }
    
    T& top()
    {
        if (empty())
        {
            throw std::out_of_range("Stack is empty");
        }
        return data.back();
    }
    
    const T& top() const
    {
        if (empty())
        {
            throw std::out_of_range("Stack is empty");
        }
        return data.back();
    }
    
    size_t size() const
    {
        return data.size();
    }
    
    void clear()
    {
        data.clear();
    }
    
    bool empty() const
    {
        return data.empty();
    }
};

#endif // STACK_H