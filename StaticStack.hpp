#ifndef _STATICSTACK_HPP_
#define _STATICSTACK_HPP_
#include "includes.hpp"
template <typename T, size_t Capacity>
class StaticStack {
public:
    bool empty() { return (top == 0); }
    void clear() { top = 0; }
    bool push(const T &item) {
        if (top == Capacity) {
            return false;
        }
        a[top++] = item;
        return true;
    }

    T pop() {
        if (top == 0) return T();
        return a[--top];
    }

    T peek() {
        if (top == 0) return T();
        return a[top - 1];
    }

    const T at(size_t pos) {
        return a[pos];
    }

private:
    T a[Capacity];
    size_t top;
};

#endif // ! _STATICSTACK_HPP_
