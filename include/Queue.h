#ifndef QUEUE_H
#define QUEUE_H

#include <vector>
#include <stdexcept>

template<class T>
class Queue {
private:
    std::vector<T> data;

public:
    Queue() = default;
    
    void push(const T& value) {
        data.push_back(value);
    }
    
    void pop() {
        if (empty()) {
            throw std::out_of_range("Queue is empty");
        }
        data.erase(data.begin());
    }
    
    T& front() {
        if (empty()) {
            throw std::out_of_range("Queue is empty");
        }
        return data.front();
    }
    
    const T& front() const {
        if (empty()) {
            throw std::out_of_range("Queue is empty");
        }
        return data.front();
    }
    
    T& back() {
        if (empty()) {
            throw std::out_of_range("Queue is empty");
        }
        return data.back();
    }
    
    const T& back() const {
        if (empty()) {
            throw std::out_of_range("Queue is empty");
        }
        return data.back();
    }
    
    bool empty() const {
        return data.empty();
    }
    
    size_t size() const {
        return data.size();
    }
    
    void clear() {
        data.clear();
    }
};

#endif // QUEUE_H