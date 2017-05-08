#ifndef THREADSAFESTACK_H
#define THREADSAFESTACK_H

#include <mutex>
#include <stack>

struct emptyStack {};

template<typename T>
class threadsafeStack {
public:
    threadsafeStack() {}
    threadsafeStack(const threadsafeStack& other) {
        std::lock_guard<std::mutex> lock(other.m);
        data = other.data;
    }
    threadsafeStack& operator= (const threadsafeStack&) = delete;
    void push(T new_value) {
        std::lock_guard<std::mutex> lock(m);
        data.push(new_value);
    }
    void pop(T& value) {
        std::lock_guard<std::mutex> lock(m);
        value = data.top();
        data.pop();
    }
    T pop() {
        std::lock_guard<std::mutex> lock(m);
        if (data.empty()) throw emptyStack();
        T res = data.top();
        data.pop();
        return res;
    }
    bool empty() const {
        std::lock_guard<std::mutex> lock(m);
        return  data.empty();
    }
private:
    std::stack<T> data;
    mutable std::mutex m;
};
#endif // !THREADSAFESTACK_H
