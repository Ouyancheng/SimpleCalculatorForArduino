#ifndef _AST_HPP_
#define _AST_HPP_
#include "includes.hpp"
#include "StaticString.hpp"
enum AST_ID {
    ast_expr,
    ast_binary,
    ast_number,
    ast_identifier,
    ast_call,
    ast_unknown
};

struct ExprAST {
    AST_ID id;
    ExprAST *parent;
    ExprAST(ExprAST *parent, AST_ID id = ast_expr) : id(id), parent(parent) {}
    virtual ~ExprAST() {}
};

struct BinaryExprAST : public ExprAST {
    char op;
    ExprAST *LHS;
    ExprAST *RHS;
    BinaryExprAST(ExprAST *parent, char op, ExprAST *LHS, ExprAST *RHS) : ExprAST(parent, ast_binary), op(op), LHS(LHS), RHS(RHS) {}
    virtual ~BinaryExprAST() {
        if (LHS) delete LHS;
        if (RHS) delete RHS;
    }
};

struct NumberExprAST : public ExprAST {
    double num;
    NumberExprAST(ExprAST *parent, double num) : ExprAST(parent, ast_number), num(num) {}
    virtual ~NumberExprAST() {}
};

struct IdentifierExprAST : public ExprAST {
    StaticString<16> name;
    IdentifierExprAST(ExprAST *parent, const StaticString<16> &name) : ExprAST(parent, ast_identifier), name(name) {}
    virtual ~IdentifierExprAST() {}
};

struct CallExprAST : public ExprAST {
    StaticString<16> identifier;
    ExprAST *args[2];
    size_t num_args;
    CallExprAST(ExprAST *parent, const StaticString<16> &identifier) :
        ExprAST(parent, ast_call), identifier(identifier), num_args(0) { args[0] = args[1] = NULL; }

    CallExprAST(ExprAST *parent, const StaticString<16> &identifier, ExprAST *first) :
        ExprAST(parent, ast_call), identifier(identifier), num_args(1) {
        args[0] = first;
        args[1] = NULL;
    }

    CallExprAST(ExprAST *parent, const StaticString<16> &identifier, ExprAST *first, ExprAST *second) :
        ExprAST(parent, ast_call), identifier(identifier), num_args(2) {
        args[0] = first;
        args[1] = second;
    }

    virtual ~CallExprAST() {
        for (size_t i = 0; i < num_args; i++) {
            if (args[i]) delete args[i];
        }
    }

};


#endif // ! _AST_HPP_
