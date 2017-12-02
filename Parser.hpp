#ifndef _PARSER_HPP_
#define _PARSER_HPP_
#include "AST.hpp"
#include "Lexer.hpp"
/*
 * <stat> ::= IDENTIFIER = <expr> // Peek the '=' sign
 *          | <expr>
 *
 * <expr> ::= <term> <expr_tail>
 *
 * <identifier_expr> ::= IDENTIFIER
 *                     | IDENTIFIER ( )                  //
 *                     | IDENTIFIER ( <expr> )           // These three are actually one kind of rule
 *                     | IDENTIFIER ( <expr> , <expr> )  // LL(*)
 *
 * // <identifier_expr> ::= IDENTIFIER
 * //                     | IDENTIFIER ( )
 * //                     | IDENTIFIER ( <expr>,* <expr> )
 *
 *
 * <expr_tail> ::= + <term> <expr_tail>
 *               | - <term> <expr_tail>
 *               | * <term> <expr_tail>
 *               | / <term> <expr_tail>
 *               | EMPTY
 *
 * <term> ::= <identifier_expr>
 *          | NUM
 *          | ( <expr> )
 */
/*
 * IDENTIFIER in {VARIABLE_NAME, sin, cos, tan, pow, abs, sqrt, max, min}
 */
namespace Parser {
ExprAST * error_expr(const char *const msg) {
#ifdef ARDUINO
    Serial.println(msg);
#else
    puts(msg);
#endif
    return NULL;
}

ExprAST * parse_statement(ExprAST *parent);

ExprAST * parse_expression(ExprAST *parent);

ExprAST * parse_expression_tail(ExprAST *parent, ExprAST *LHS, int min_priority);

ExprAST * parse_parenthesis_expression(ExprAST *parent);

ExprAST * parse_number_expression(ExprAST *parent);

ExprAST * parse_identifier_expression(ExprAST *parent);

ExprAST * parse_term_expression(ExprAST *parent);

ExprAST * parse_number_expression(ExprAST *parent) {
    ExprAST *p = new NumberExprAST(parent, Lexer::current_tok.num);
    Lexer::get_next_token();
    // error_expr("Generated number");
    return p;
}

ExprAST * parse_parenthesis_expression(ExprAST *parent) {
    Lexer::get_next_token(); // eat '('
    ExprAST *p = parse_expression(parent);
    if (!p) return NULL;
    if (Lexer::current_tok.id != Lexer::tok_character || Lexer::current_tok.c != ')') return error_expr("Parathesis mismatch");
    Lexer::get_next_token(); // eat ')'
    return p;
}

ExprAST * parse_identifier_expression(ExprAST *parent) {
    StaticString<16> str = Lexer::current_tok.str;
    Lexer::get_next_token(); // consume current token
    if (Lexer::current_tok.id != Lexer::tok_character || Lexer::current_tok.c != '(') {
        return new IdentifierExprAST(parent, str); // <identifier_expression> ::= IDENTIFIER
    }

    Lexer::get_next_token(); // eat '('
    ExprAST *args[2] = {NULL, NULL};

    if (Lexer::current_tok.id == Lexer::tok_character && Lexer::current_tok.c == ')') {
        Lexer::get_next_token(); // eat ')'
        return new CallExprAST(parent, str);
    }

    args[0] = parse_expression(NULL);
    if (!args[0]) return error_expr("Argument parsing error");

    if (Lexer::current_tok.id == Lexer::tok_character && Lexer::current_tok.c == ')') {
        Lexer::get_next_token(); // eat ')'
        CallExprAST *p = new CallExprAST(parent, str, args[0]);
        p->args[0]->parent = p;
        return p;
    }

    if (Lexer::current_tok.id != Lexer::tok_character || Lexer::current_tok.c != ',') {
        delete args[0];
        return error_expr("Missing ','");
    }

    Lexer::get_next_token(); // eat ','

    args[1] = parse_expression(NULL);
    if (!args[1]) return error_expr("Argument 2 parsing error");

    if (Lexer::current_tok.id == Lexer::tok_character && Lexer::current_tok.c == ')') {
        Lexer::get_next_token(); // eat ')'
        CallExprAST *p = new CallExprAST(parent, str, args[0], args[1]);
        p->args[0]->parent = p;
        p->args[1]->parent = p;
        return p;
    }

    delete args[0];
    delete args[1];

    return error_expr("Function supports no more than 2 arguments");

}

ExprAST * parse_term_expression(ExprAST *parent) {
    if (Lexer::current_tok.id == Lexer::tok_character && Lexer::current_tok.c == '(') {
        return parse_parenthesis_expression(parent);
    } else if (Lexer::current_tok.id == Lexer::tok_number) {
        return parse_number_expression(parent);
    } else if (Lexer::current_tok.id == Lexer::tok_identifier) {
        return parse_identifier_expression(parent);
    } else {
        return error_expr("Unknown term expression");
    }
}

int get_priority(const char c) {
    switch (c) {
    case '+':
    case '-':
        return 30;
    case '*':
    case '/':
        return 60;
    case '=':
        return -1;
    default:
        return -1;
    }
}

ExprAST * parse_expression(ExprAST *parent) {
    ExprAST *LHS = parse_term_expression(NULL);
    if (!LHS) return error_expr("LHS syntax error");
    ExprAST *q = parse_expression_tail(parent, LHS, 0);
    if (!q) {
        delete LHS;
        return error_expr("RHS syntax error");
    }
    return q;
}

ExprAST * parse_expression_tail(ExprAST *parent, ExprAST *LHS, int min_priority) {
    while (true) {
        // cout << Lexer::current_tok.id << endl;
        if (Lexer::current_tok.id == Lexer::tok_end ||
                (Lexer::current_tok.id == Lexer::tok_character &&
                 (Lexer::current_tok.c == ',' || Lexer::current_tok.c == ')' || Lexer::current_tok.c == '\0'))) {
            LHS->parent = parent;
            return LHS;
        } // Terminating
        char op = Lexer::current_tok.c;
        int op_priority = get_priority(op);
        if (op_priority < 0) {
            return error_expr("Unknown operator");
        } // tackle unknown operator
        if (op_priority < min_priority) {
            LHS->parent = parent;
            return LHS; // If the current operator priority is less than the minimum priority, the function returns LHS
            // Consider 1 + 2 * 3 + 6
            // when parsing + 6, * has a higher priority than +. The function directly returns LHS
            /*
             * AST:
             *          +
             *        /   \
             *       +     6
             *      / \
             *     1   *
             *        / \
             *       2   3
             */
            // When parsing + 6, our min_priority should be higher than the priority of *. In this time, LHS is exactly the root.
        } // This can also tackle the unknown operators

        Lexer::get_next_token(); // eat operator
        // Now it's time for us to parse the next term
        ExprAST *RHS = parse_term_expression(NULL);

        if (!RHS) {
            return error_expr("Tail RHS syntax error");
        }

        int next_op_priority = -1;
        if (Lexer::current_tok.id == Lexer::tok_character) {
            next_op_priority = get_priority(Lexer::current_tok.c);
        }

        if (op_priority < next_op_priority) {
            // Still consider 1 + 2 * 3 + 6
            // when parsing * 3, we know * has a higher priority than +, then 2 * 3 should be the right subtree of the +.
            /*
             * AST:
             *          +
             *        /   \
             *       +     6
             *      / \
             *     1   *
             *        / \
             *       2   3
             */
            RHS = parse_expression_tail(NULL, RHS, op_priority + 1);
            if (!RHS) {
                return error_expr("higher priority RHS error");
            }
        }

        LHS = new BinaryExprAST(NULL, op, LHS, RHS);
        ((BinaryExprAST *)LHS)->LHS->parent = LHS;
        ((BinaryExprAST *)LHS)->RHS->parent = RHS;

    }
}

ExprAST * parse_statement(ExprAST *parent) {
    Lexer::Token next_tok = Lexer::peek_next_token();
    if (next_tok.id == Lexer::tok_character && next_tok.c == '=') { // LL(2)
        if (Lexer::current_tok.id != Lexer::tok_identifier) {
            return error_expr("rvalue assignment");
        }
        ExprAST *LHS = parse_identifier_expression(NULL);
        Lexer::get_next_token(); // eat '='
        ExprAST * RHS = parse_expression(NULL);
        if (!RHS) {
            delete LHS;
            return error_expr("assignment RHS syntax error");
        }
        BinaryExprAST *p = new BinaryExprAST(parent, '=', LHS, RHS);
        LHS->parent = p;
        RHS->parent = p;
        return p;
    }
    return parse_expression(parent);
}





}
#endif // ! _PARSER_HPP_
