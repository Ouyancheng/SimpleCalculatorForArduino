#ifndef _STATICARRAY_HPP_
#define _STATICARRAY_HPP_
#include "includes.hpp"
template <typename T, size_t Capacity>
class StaticArray {
private:
    T a[Capacity + 1];
    size_t length;
public:
    class iterator_prototype;
    StaticArray() : length(0) {}
    bool push_back(const T &item) {
        if (length >= Capacity) {
            return false;
        }
        a[length++] = item;
        return true;
    }
    bool pop_back() {
        if (length <= 0) return false;
        --length;
        return true;
    }

    iterator_prototype begin() {
        return iterator_prototype(a);
    }
    iterator_prototype end() {
        return iterator_prototype(a + length);
    }

    iterator_prototype find(const T &item) {
        iterator_prototype i;
        for (i = a; i < a + length; i++) {
            if (item == *i) {
                return iterator_prototype(i);
            }
        }
        return end();
    }

    T & operator [] (size_t pos) {
        return a[pos];
    }

    iterator_prototype find_or_insert(const T &item) {
        iterator_prototype iter = find(item);
        if (iter == end()) {
            push_back(item);
            return iterator_prototype(a + length - 1);
        } else {
            return iter;
        }
    }

    size_t size() {
        return length;
    }

    size_t capacity() {
        return Capacity;
    }

    void clear() {
        length = 0;
    }


public:
    class iterator_prototype {
    private:
        T *p;
    public:
        iterator_prototype() : p(NULL) {}
        iterator_prototype(T *p) : p(p) {}
        iterator_prototype operator ++ () {
            ++p;
            return *this;
        }
        iterator_prototype operator ++ (int) {
            iterator_prototype temp = *this;
            ++p;
            return temp;
        }
        iterator_prototype operator -- () {
            --p;
            return *this;
        }
        iterator_prototype operator -- (int) {
            iterator_prototype temp = *this;
            --p;
            return temp;
        }

        T & operator * () {
            return *p;
        }

        T * operator ->() {
            return p;
        }

        bool operator == (const iterator_prototype &another) const {
            return p == another.p;
        }

        bool operator != (const iterator_prototype &another) const {
            return p != another.p;
        }

        bool operator <= (const iterator_prototype &another) const {
            return p <= another.p;
        }

        bool operator >= (const iterator_prototype &another) const {
            return p >= another.p;
        }

        bool operator < (const iterator_prototype &another) const {
            return p < another.p;
        }

        bool operator > (const iterator_prototype &another) const {
            return p > another.p;
        }

    };
};

#endif // ! _STATICARRAY_HPP_
