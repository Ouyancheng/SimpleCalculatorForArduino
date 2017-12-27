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
 * <term> ::= <positive_term>
 *          | - <positive_term>
 *          | + <positive_term> 
 *
 * <positive_term> ::= <identifier_expr>
 *                   | NUM
 *                   | ( <expr> )
 */
/*
 * IDENTIFIER in {VARIABLE_NAME, sin, cos, tan, pow, abs, sqrt, max, min}
 */
class Parser {
public:
    // Prompt error
    static ExprAST * error_expr(const char *const msg) {
#ifdef ARDUINO
        Serial.println(msg);
#else
        puts(msg);
#endif
        return NULL;
    }
    
    static ExprAST * parse_number_expression(ExprAST *parent) {
        ExprAST *p = new NumberExprAST(parent, Lexer::current_tok.num);
        Lexer::get_next_token();
        // error_expr("Generated number");
        return p;
    }
    
    static ExprAST * parse_parenthesis_expression(ExprAST *parent) {
        Lexer::get_next_token(); // eat '('
        ExprAST *p = parse_expression(parent);
        if (!p) return NULL;
        if (Lexer::current_tok.id != Lexer::tok_character || Lexer::current_tok.c != ')') return error_expr("parse_parenthesis_expression: Parathesis mismatch");
        Lexer::get_next_token(); // eat ')'
        return p;
    }
    
    static ExprAST * parse_identifier_expression(ExprAST *parent) {
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
        if (!args[0]) return error_expr("parse_identifier_expression: Argument parsing error");
        
        if (Lexer::current_tok.id == Lexer::tok_character && Lexer::current_tok.c == ')') {
            Lexer::get_next_token(); // eat ')'
            CallExprAST *p = new CallExprAST(parent, str, args[0]);
            p->args[0]->parent = p;
            return p;
        }
        
        if (Lexer::current_tok.id != Lexer::tok_character || Lexer::current_tok.c != ',') {
            delete args[0];
            return error_expr("parse_identifier_expression: Missing ','");
        }
        
        Lexer::get_next_token(); // eat ','
        
        args[1] = parse_expression(NULL);
        if (!args[1]) return error_expr("parse_identifier_expression: Argument 2 parsing error");
        
        if (Lexer::current_tok.id == Lexer::tok_character && Lexer::current_tok.c == ')') {
            Lexer::get_next_token(); // eat ')'
            CallExprAST *p = new CallExprAST(parent, str, args[0], args[1]);
            p->args[0]->parent = p;
            p->args[1]->parent = p;
            return p;
        }
        
        delete args[0];
        delete args[1];
        
        return error_expr("parse_identifier_expression: Function supports no more than 2 arguments");
        
    }
    
    static ExprAST * parse_positive_term_expression(ExprAST *parent) {
        if (Lexer::current_tok.id == Lexer::tok_character && Lexer::current_tok.c == '(') {
            return parse_parenthesis_expression(parent);
        } else if (Lexer::current_tok.id == Lexer::tok_number) {
            return parse_number_expression(parent);
        } else if (Lexer::current_tok.id == Lexer::tok_identifier) {
            return parse_identifier_expression(parent);
        } else {
            return error_expr("parse_positive_term_expression: Unknown positive term expression");
        }
    }
    
    static ExprAST * parse_term_expression(ExprAST *parent) {
        if (Lexer::current_tok.id == Lexer::tok_character && Lexer::current_tok.c == '-') {
            Lexer::get_next_token(); // eat '-'
            ExprAST *expr = parse_positive_term_expression(NULL);
            if (!expr) {
                return error_expr("parse_term_expression: Unknown term expression");
            }
            ExprAST *p = new CallExprAST(parent, "neg", expr, NULL);
            // error_expr("Constructed neg");
            expr->parent = p;
            return p;
        } else if (Lexer::current_tok.id == Lexer::tok_character && Lexer::current_tok.c == '+') {
            Lexer::get_next_token(); // eat '+'
            ExprAST *expr = parse_positive_term_expression(parent);
            if (!expr) {
                return error_expr("parse_term_expression: Unknown term expression");
            }
            return expr;
        }
        else {
            ExprAST *expr = parse_positive_term_expression(parent);
            if (!expr) {
                return error_expr("parse_term_expression: Unknown term expression");
            }
            return expr;
        }
    }
    // returns the precedence of the operator
    static int get_priority(const char c) {
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
    
    static ExprAST * parse_expression(ExprAST *parent) {
        ExprAST *LHS = parse_term_expression(NULL);
        if (!LHS) return error_expr("parse_expression: LHS syntax error");
        ExprAST *q = parse_expression_tail(parent, LHS, 0);
        if (!q) {
            // delete LHS;
            return error_expr("parse_expression: RHS syntax error");
        }
        return q;
    }
    // This is a bottom-up approach to construct AST for left associative operator, following the precedence.
    static ExprAST * parse_expression_tail(ExprAST *parent, ExprAST *LHS, int min_priority) {
        while (true) {
            // cout << Lexer::current_tok.id << endl;
            if (Lexer::current_tok.id != Lexer::tok_character ||
                (Lexer::current_tok.id == Lexer::tok_character &&
                 (Lexer::current_tok.c == ',' || Lexer::current_tok.c == ')' || Lexer::current_tok.c == '\0'))) {
                    LHS->parent = parent;
                    return LHS;
                } // Terminating
            
            char op = Lexer::current_tok.c;
            // cout << op << endl;
            int op_priority = get_priority(op);
            
            if (op_priority < 0) {
                return error_expr("parse_expression_tail: Unknown operator");
            } // tackle unknown operator
            
            if (op_priority < min_priority) {
                if (LHS) LHS->parent = parent;
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
                delete LHS;
                return error_expr("parse_expression_tail: Tail RHS syntax error");
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
                    delete LHS;
                    return error_expr("parse_expression_tail: higher priority RHS error");
                }
            }
            
            LHS = new BinaryExprAST(NULL, op, LHS, RHS);
            ((BinaryExprAST *)LHS)->LHS->parent = LHS;
            ((BinaryExprAST *)LHS)->RHS->parent = RHS;
            
        }
    }
    
    static ExprAST * parse_statement(ExprAST *parent) {
        Lexer::Token next_tok = Lexer::peek_next_token();
        if (next_tok.id == Lexer::tok_character && next_tok.c == '=') { // LL(2)
            if (Lexer::current_tok.id != Lexer::tok_identifier) {
                return error_expr("parse_statement: rvalue assignment");
            }
            ExprAST *LHS = parse_identifier_expression(NULL);
            Lexer::get_next_token(); // eat '='
            ExprAST * RHS = parse_expression(NULL);
            if (!RHS) {
                delete LHS;
                return error_expr("parse_statement: assignment RHS syntax error");
            }
            BinaryExprAST *p = new BinaryExprAST(parent, '=', LHS, RHS);
            LHS->parent = p;
            RHS->parent = p;
            return p;
        }
        return parse_expression(parent);
    }
    
    
    
    

};
#endif // ! _PARSER_HPP_
