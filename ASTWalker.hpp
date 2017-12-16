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

#define BIN_OP(va, vb) Symbol b = s.pop(); \
                        Symbol a = s.pop(); \
                        double va; \
                        double vb; \
                        if (a.is_var) { \
                            StaticArray<Symbol, 16>::iterator_prototype iter = symbol_table.find(a); \
                            if (iter == symbol_table.end()) { \
                                error_exec("ASTWalker: Undefined symbol"); \
                                return; \
                            } \
                            va = iter->value; \
                        } else { \
                            va = a.value; \
                        } \
                        if (b.is_var) { \
                            StaticArray<Symbol, 16>::iterator_prototype iter = symbol_table.find(b); \
                            if (iter == symbol_table.end()) { \
                                error_exec("ASTWalker: Undefined symbol"); \
                                return; \
                            } \
                            vb = iter->value; \
                        } else { \
                            vb = b.value; \
                        }

#define SINGLE_ARG_FUNCION(va) Symbol a = s.pop(); \
                                double va; \
                                if (a.is_var) { \
                                    StaticArray<Symbol, 16>::iterator_prototype iter = symbol_table.find(a); \
                                    if (iter == symbol_table.end()) { \
                                        error_exec("ASTWalker: Undefined symbol"); \
                                        return; \
                                    } \
                                    va = iter->value; \
                                } else { \
                                    va = a.value; \
                                }

#define DOUBLE_ARGS_FUNCTION(va, vb) Symbol b = s.pop(); \
                                        Symbol a = s.pop(); \
                                        double va; \
                                        double vb; \
                                        if (a.is_var) { \
                                            StaticArray<Symbol, 16>::iterator_prototype iter = symbol_table.find(a); \
                                            if (iter == symbol_table.end()) { \
                                                error_exec("ASTWalker: Undefined symbol"); \
                                                return; \
                                            } \
                                            va = iter->value; \
                                        } else { \
                                            va = a.value; \
                                        } \
                                        if (b.is_var) { \
                                            StaticArray<Symbol, 16>::iterator_prototype iter = symbol_table.find(b); \
                                            if (iter == symbol_table.end()) { \
                                                error_exec("ASTWalker: Undefined symbol"); \
                                                return; \
                                            } \
                                            vb = iter->value; \
                                        } else { \
                                            vb = b.value; \
                                        }

#define MATCH_FUNCTION_NAME(name) ((CallExprAST *)(root))->identifier == name
#define MATCH_BINARY_OPERATOR(operator) ((BinaryExprAST *)root)->op == operator

void ast_post_order_traverse(ExprAST *root) {
    if (root) {
        switch (root->id) {
        case ast_binary:
            ast_post_order_traverse(((BinaryExprAST *)root)->LHS);
            ast_post_order_traverse(((BinaryExprAST *)root)->RHS);
//            cout << ((BinaryExprAST *)root)->op << " ";
            if (MATCH_BINARY_OPERATOR('+')) {
                BIN_OP(va, vb);
                s.push(va + vb);
            } else if (MATCH_BINARY_OPERATOR('-')) {
                BIN_OP(va, vb);
                s.push(va - vb);
            } else if (MATCH_BINARY_OPERATOR('*')) {
                BIN_OP(va, vb);
                s.push(va * vb);
            } else if (MATCH_BINARY_OPERATOR('/')) {
                BIN_OP(va, vb);
                s.push(va / vb);
            } else if (MATCH_BINARY_OPERATOR('=')) {
                Symbol v = s.pop();
                Symbol k = s.pop();
                if (!k.is_var) {
                    error_exec("ASTWalker: rvalue assignment");
                    return;
                }
                if (v.is_var) {
                    // error_exec("Symbol not defined");
                    StaticArray<Symbol, 16>::iterator_prototype iter = symbol_table.find(Symbol(v.key));
                    if (iter == symbol_table.end()) {
                        error_exec("ASTWalker: Undefined symbol");
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
            if (MATCH_FUNCTION_NAME("sin")) {
                SINGLE_ARG_FUNCION(va);
                s.push(sin(va));
            } else if (MATCH_FUNCTION_NAME("cos")) {
                SINGLE_ARG_FUNCION(va);
                s.push(cos(va));
            } else if (MATCH_FUNCTION_NAME("tan")) {
                SINGLE_ARG_FUNCION(va);
                s.push(tan(va));
            } else if (MATCH_FUNCTION_NAME("pow")) {
                DOUBLE_ARGS_FUNCTION(va, vb);
                s.push(pow(va, vb));
            } else if (MATCH_FUNCTION_NAME("log2")) {
                SINGLE_ARG_FUNCION(va);
                s.push(log10(va) / log10(2));
            } else if (MATCH_FUNCTION_NAME("log10")) {
                SINGLE_ARG_FUNCION(va);
                s.push(log10(va));
            } else if (MATCH_FUNCTION_NAME("log")) {
                DOUBLE_ARGS_FUNCTION(va, vb);
                s.push(log10(vb) / log10(va));
            } else if (MATCH_FUNCTION_NAME("abs")) {
                SINGLE_ARG_FUNCION(va);
                s.push(fabs(va));
            } else if (MATCH_FUNCTION_NAME("sqrt")) {
                SINGLE_ARG_FUNCION(va);
                s.push(sqrt(va));
            } else if (MATCH_FUNCTION_NAME("max")) {
                DOUBLE_ARGS_FUNCTION(va, vb);
                s.push(max(va, vb));
            } else if (MATCH_FUNCTION_NAME("min")) {
                DOUBLE_ARGS_FUNCTION(va, vb);
                s.push(min(va, vb));
            } else if (MATCH_FUNCTION_NAME("neg")) {
                SINGLE_ARG_FUNCION(va);
                s.push(-va);
            } else {
                error_exec("ASTWalker: Unknown identifier");
                return;
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
