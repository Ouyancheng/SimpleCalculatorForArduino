#ifndef _ASTWALKER_HPP_
#define _ASTWALKER_HPP_
#include "includes.hpp"
#include "AST.hpp"
#include "StaticStack.hpp"
#include "StaticArray.hpp"
struct Symbol {
    StaticString<16> key;
    double value;
    bool is_var;
    bool operator == (const Symbol &another) const {
        return key == another.key;
    }

    Symbol(const StaticString<16> &key, double value) : key(key), value(value), is_var(true) {}
    Symbol(const StaticString<16> &key) : key(key), value(0.0), is_var(true) {}
    Symbol(double value) : value(value), is_var(false) {}
    Symbol() : value(0.0), is_var(true) {}
};

StaticStack<Symbol, 64> s;
StaticArray<Symbol, 16> symbol_table;

void error_exec(const char *const str) {
#ifdef ARDUINO
    Serial.println(str);
#else
    puts(str);
#endif
}

void ast_post_order_traverse(ExprAST *root) {
    if (root) {
        switch (root->id) {
        case ast_binary:
            ast_post_order_traverse(((BinaryExprAST *)root)->LHS);
            ast_post_order_traverse(((BinaryExprAST *)root)->RHS);
//            cout << ((BinaryExprAST *)root)->op << " ";
            if (((BinaryExprAST *)root)->op == '+') {
                Symbol b = s.pop();
                Symbol a = s.pop();
                double va;
                double vb;
                if (a.is_var) {
                    StaticArray<Symbol, 16>::iterator_prototype iter = symbol_table.find(a);
                    if (iter == symbol_table.end()) {
                        error_exec("Undefined symbol");
                        return;
                    }
                    va = iter->value;
                } else {
                    va = a.value;
                }
                if (b.is_var) {
                    StaticArray<Symbol, 16>::iterator_prototype iter = symbol_table.find(b);
                    if (iter == symbol_table.end()) {
                        error_exec("Undefined symbol");
                        return;
                    }
                    vb = iter->value;
                } else {
                    vb = b.value;
                }
                s.push(va + vb);
            } else if (((BinaryExprAST *)root)->op == '-') {
                Symbol b = s.pop();
                Symbol a = s.pop();
                double va;
                double vb;
                if (a.is_var) {
                    StaticArray<Symbol, 16>::iterator_prototype iter = symbol_table.find(a);
                    if (iter == symbol_table.end()) {
                        error_exec("Undefined symbol");
                        return;
                    }
                    va = iter->value;
                } else {
                    va = a.value;
                }
                if (b.is_var) {
                    StaticArray<Symbol, 16>::iterator_prototype iter = symbol_table.find(b);
                    if (iter == symbol_table.end()) {
                        error_exec("Undefined symbol");
                        return;
                    }
                    vb = iter->value;
                } else {
                    vb = b.value;
                }
                s.push(va - vb);
            } else if (((BinaryExprAST *)root)->op == '*') {
                Symbol b = s.pop();
                Symbol a = s.pop();
                double va;
                double vb;
                if (a.is_var) {
                    StaticArray<Symbol, 16>::iterator_prototype iter = symbol_table.find(a);
                    if (iter == symbol_table.end()) {
                        error_exec("Undefined symbol");
                        return;
                    }
                    va = iter->value;
                } else {
                    va = a.value;
                }
                if (b.is_var) {
                    StaticArray<Symbol, 16>::iterator_prototype iter = symbol_table.find(b);
                    if (iter == symbol_table.end()) {
                        error_exec("Undefined symbol");
                        return;
                    }
                    vb = iter->value;
                } else {
                    vb = b.value;
                }
                s.push(va * vb);
            } else if (((BinaryExprAST *)root)->op == '/') {
                Symbol b = s.pop();
                Symbol a = s.pop();
                double va;
                double vb;
                if (a.is_var) {
                    StaticArray<Symbol, 16>::iterator_prototype iter = symbol_table.find(a);
                    if (iter == symbol_table.end()) {
                        error_exec("Undefined symbol");
                        return;
                    }
                    va = iter->value;
                } else {
                    va = a.value;
                }
                if (b.is_var) {
                    StaticArray<Symbol, 16>::iterator_prototype iter = symbol_table.find(b);
                    if (iter == symbol_table.end()) {
                        error_exec("Undefined symbol");
                        return;
                    }
                    vb = iter->value;
                } else {
                    vb = b.value;
                }
                s.push(va / vb);
            } else if (((BinaryExprAST *)root)->op == '=') {
                Symbol v = s.pop();
                Symbol k = s.pop();
                if (!k.is_var) {
                    error_exec("rvalue assignment");
                    return;
                }
                if (v.is_var) {
                    // error_exec("Symbol not defined");
                    StaticArray<Symbol, 16>::iterator_prototype iter = symbol_table.find(Symbol(v.key));
                    if (iter == symbol_table.end()) {
                        error_exec("Undefined symbol");
                        return;
                    }
                    (symbol_table.find_or_insert(Symbol(k)))->value = iter->value;
                    s.push(iter->value);

                } else {
                    (symbol_table.find_or_insert(Symbol(k)))->value = v.value;
                    s.push(v.value);
                }
            }
            break;
        case ast_number:
//            cout << ((NumberExprAST *)(root))->num << " ";
            s.push(((NumberExprAST *)(root))->num);
            break;
        case ast_identifier:
//            ((IdentifierExprAST *)(root))->name.print();

            // StaticArray<Symbol, 16>::iterator_prototype iter = symbol_table.find(Symbol(((IdentifierExprAST *)root)->name));
            s.push(Symbol(((IdentifierExprAST *)(root))->name));


            break;
        case ast_call:
            for (size_t i = 0; i < ((CallExprAST *)root)->num_args; i++) {
                ast_post_order_traverse(((CallExprAST *)(root))->args[i]);
            }
//            ((CallExprAST *)(root))->identifier.print();
            if (((CallExprAST *)(root))->identifier == "sin") {
                // double a = s.pop();
                Symbol a = s.pop();
                double va;
                if (a.is_var) {
                    StaticArray<Symbol, 16>::iterator_prototype iter = symbol_table.find(a);
                    if (iter == symbol_table.end()) {
                        error_exec("Undefined symbol");
                        return;
                    }
                    va = iter->value;
                } else {
                    va = a.value;
                }

                s.push(sin(va));
            } else if (((CallExprAST *)(root))->identifier == "cos") {
                Symbol a = s.pop();
                double va;
                if (a.is_var) {
                    StaticArray<Symbol, 16>::iterator_prototype iter = symbol_table.find(a);
                    if (iter == symbol_table.end()) {
                        error_exec("Undefined symbol");
                        return;
                    }
                    va = iter->value;
                } else {
                    va = a.value;
                }

                s.push(cos(va));
            } else if (((CallExprAST *)(root))->identifier == "tan") {
                Symbol a = s.pop();
                double va;
                if (a.is_var) {
                    StaticArray<Symbol, 16>::iterator_prototype iter = symbol_table.find(a);
                    if (iter == symbol_table.end()) {
                        error_exec("Undefined symbol");
                        return;
                    }
                    va = iter->value;
                } else {
                    va = a.value;
                }

                s.push(tan(va));
            } else if (((CallExprAST *)(root))->identifier == "pow") {
                Symbol b = s.pop();
                Symbol a = s.pop();
                double va;
                double vb;
                if (a.is_var) {
                    StaticArray<Symbol, 16>::iterator_prototype iter = symbol_table.find(a);
                    if (iter == symbol_table.end()) {
                        error_exec("Undefined symbol");
                        return;
                    }
                    va = iter->value;
                } else {
                    va = a.value;
                }
                if (b.is_var) {
                    StaticArray<Symbol, 16>::iterator_prototype iter = symbol_table.find(b);
                    if (iter == symbol_table.end()) {
                        error_exec("Undefined symbol");
                        return;
                    }
                    vb = iter->value;
                } else {
                    vb = b.value;
                }
                s.push(pow(va, vb));
            } else if (((CallExprAST *)(root))->identifier == "log2") {
                Symbol a = s.pop();
                double va;
                if (a.is_var) {
                    StaticArray<Symbol, 16>::iterator_prototype iter = symbol_table.find(a);
                    if (iter == symbol_table.end()) {
                        error_exec("Undefined symbol");
                        return;
                    }
                    va = iter->value;
                } else {
                    va = a.value;
                }

                s.push(log10(va)/log10(2));
            } else if (((CallExprAST *)(root))->identifier == "log10") {
                Symbol a = s.pop();
                double va;
                if (a.is_var) {
                    StaticArray<Symbol, 16>::iterator_prototype iter = symbol_table.find(a);
                    if (iter == symbol_table.end()) {
                        error_exec("Undefined symbol");
                        return;
                    }
                    va = iter->value;
                } else {
                    va = a.value;
                }

                s.push(log10(va));
            } else if (((CallExprAST *)(root))->identifier == "abs") {
                Symbol a = s.pop();
                double va;
                if (a.is_var) {
                    StaticArray<Symbol, 16>::iterator_prototype iter = symbol_table.find(a);
                    if (iter == symbol_table.end()) {
                        error_exec("Undefined symbol");
                        return;
                    }
                    va = iter->value;
                } else {
                    va = a.value;
                }

                s.push(fabs(va));
            } else if (((CallExprAST *)(root))->identifier == "sqrt") {
                Symbol a = s.pop();
                double va;
                if (a.is_var) {
                    StaticArray<Symbol, 16>::iterator_prototype iter = symbol_table.find(a);
                    if (iter == symbol_table.end()) {
                        error_exec("Undefined symbol");
                        return;
                    }
                    va = iter->value;
                } else {
                    va = a.value;
                }

                s.push(sqrt(va));
            } else if (((CallExprAST *)(root))->identifier == "max") {
                Symbol b = s.pop();
                Symbol a = s.pop();
                double va;
                double vb;
                if (a.is_var) {
                    StaticArray<Symbol, 16>::iterator_prototype iter = symbol_table.find(a);
                    if (iter == symbol_table.end()) {
                        error_exec("Undefined symbol");
                        return;
                    }
                    va = iter->value;
                } else {
                    va = a.value;
                }
                if (b.is_var) {
                    StaticArray<Symbol, 16>::iterator_prototype iter = symbol_table.find(b);
                    if (iter == symbol_table.end()) {
                        error_exec("Undefined symbol");
                        return;
                    }
                    vb = iter->value;
                } else {
                    vb = b.value;
                }
                s.push(max(va, vb));
            } else if (((CallExprAST *)(root))->identifier == "min") {
                Symbol b = s.pop();
                Symbol a = s.pop();
                double va;
                double vb;
                if (a.is_var) {
                    StaticArray<Symbol, 16>::iterator_prototype iter = symbol_table.find(a);
                    if (iter == symbol_table.end()) {
                        error_exec("Undefined symbol");
                        return;
                    }
                    va = iter->value;
                } else {
                    va = a.value;
                }
                if (b.is_var) {
                    StaticArray<Symbol, 16>::iterator_prototype iter = symbol_table.find(b);
                    if (iter == symbol_table.end()) {
                        error_exec("Undefined symbol");
                        return;
                    }
                    vb = iter->value;
                } else {
                    vb = b.value;
                }
                s.push(min(va, vb));
            }
            break;
        default:
            break;
        }
    }
}

//class ASTWalker {
//public:
//    ASTWalker(ExprAST *root) {

//    }
//private:
//    ExprAST *p;
//};

#endif // ! _ASTWALKER_HPP_
