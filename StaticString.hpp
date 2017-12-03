#ifndef _STATICSTRING_HPP_
#define _STATICSTRING_HPP_
#include "includes.hpp"
template <size_t Capacity>
class StaticString {
private:
    char a[Capacity + 1]; // don't need the terminating character
    size_t length;
    static char dummy;

    bool c_str_construct(const char *const str) {
        length = 0;
        size_t i = 0;
        while (i < Capacity && str[i] != '\0') {
            a[i] = str[i];
            ++i;
            ++length;
        }
        return (i < Capacity);
    }

    template <size_t Another_Capacity>
    bool copy_construct(const StaticString<Another_Capacity> &another) {
        length = 0;
        size_t i = 0;
        while (i < Capacity && i < another.length) {
            a[i] = another.a[i];
            ++i;
            ++length;
        }
        return (another.length < Capacity);
    }

public:
    StaticString() : length(0) {

    }

    StaticString(const char c) {
        a[0] = c;
        length = 1;
    }

    StaticString(const char *const str) {
        c_str_construct(str);
    }

    StaticString operator = (const char *const str) {
        c_str_construct(str);
        return *this;
    }

    template <size_t Another_Capacity>
    StaticString(const StaticString<Another_Capacity> &another) {
        copy_construct(another);
    }

    template <size_t Another_Capacity>
    StaticString operator = (const StaticString<Another_Capacity> &another) {
        copy_construct(another);
        return *this;
    }

    size_t size() const {
        return length;
    }

    bool append(const char c) {
        if (length < Capacity) {
            a[length++] = c;
            return true;
        } else {
            return false;
        }
    }

    bool pop() {
        if (length == 0) return false;
        --length;
        return true;
    }

    void operator += (const char c) {
        append(c);
    }

    StaticString operator + (const char c) {
        append(c);
        return *this;
    }

    template <size_t Another_Capacity>
    bool append(const StaticString<Another_Capacity> &another) {
        if (length + another.length - 1 < Capacity) {
            size_t i = length;
            size_t j = 0;
            while (j < another.length && i < Capacity) {
                a[i] = another.a[j];
                ++i;
                ++j;
                ++length;
            }
            return true;
        } else {
            return false;
        }
    }

    template <size_t Another_Capacity>
    void operator += (const StaticString<Another_Capacity> &another) {
        append(another);
    }

    template <size_t Another_Capacity>
    StaticString operator +(const StaticString<Another_Capacity> &another) {
        append(another);
        return *this;
    }

    bool append(const char *const another) {
        if (length + strlen(another) < Capacity) {
            size_t i = length;
            size_t j = 0;
            while (j < strlen(another) && i < Capacity) {
                a[i] = another[j];
                ++i;
                ++j;
                ++length;
            }
            return true;
        } else {
            return false;
        }
    }

    void operator += (const char *const another) {
        append(another);
    }

    StaticString operator +(const char *const another) {
        append(another);
        return *this;
    }

    char & operator [] (size_t pos) {
        return a[pos];
    }

    char & at(size_t pos) {
        if (pos >= length) {
            dummy = '\0';
            return dummy;
        } else {
            return a[pos];
        }
    }

    char operator [] (size_t pos) const {
        return a[pos];
    }

    char at(size_t pos) const {
        if (pos >= length) {
            return '\0';
        } else {
            return a[pos];
        }
    }

    template <size_t Another_Capacity>
    bool operator == (const StaticString<Another_Capacity> &another) const {
        if (length != another.length) return false;
        for (size_t i = 0; i < length; i++) {
            if (a[i] != another.a[i]) {
                return false;
            }
        }
        return true;
    }

    bool operator == (const char *const str) const {
        size_t another_length = strlen(str);
        if (length != another_length) return false;
        for (size_t i = 0; i < length; i++) {
            if (a[i] != str[i]) {
                return false;
            }
        }
        return true;
    }

    double to_double() {
        char temp[length + 1];
        for (size_t i = 0; i < length; i++) {
            temp[i] = a[i];
        }
        temp[length] = '\0';
        return atof(temp);
    }

    bool empty() {
        return (length == 0);
    }

    void clear() {
        length = 0;
    }

    size_t capacity() {
        return Capacity;
    }

    const char * const c_str() {
        a[length] = '\0';
        return a;
    }

    static size_t strlen(const char *const str) {
        size_t i = 0;
        while (str[i] != '\0') ++i;
        return i;
    }

#ifndef ARDUINO
    void print() {
        char temp[Capacity + 1];
        for (size_t i = 0; i < length; i++) {
            temp[i] = a[i];
        }
        temp[length] = '\0';
        puts(temp);
    }

#else
    void print() {
        char temp[Capacity + 1];
        for (size_t i = 0; i < length; i++) {
            temp[i] = a[i];
        }
        temp[length] = '\0';
        Serial.println(temp);
    }
#endif
};

#endif // ! _STATICSTRING_HPP_
