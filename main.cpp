#include "includes.hpp"
#include "StaticString.hpp"
#include "Lexer.hpp"
#include "Parser.hpp"
#include "ASTWalker.hpp"
using namespace std;
//void ast_traverse(ExprAST *root) {
//    if (root) {
//        switch (root->id) {
//        case ast_binary:
//            ast_traverse(((BinaryExprAST *)(root))->LHS);
//            ast_traverse(((BinaryExprAST *)(root))->RHS);
//            cout << ((BinaryExprAST *)(root))->op << " ";
//            break;
//        case ast_number:
//            cout << ((NumberExprAST *)(root))->num << " ";
//            break;
//        case ast_identifier:
//            ((IdentifierExprAST *)(root))->name.print();
//            break;
//        case ast_call:
//            for (size_t i = 0; i < ((CallExprAST *)root)->num_args; i++) {
//                ast_traverse(((CallExprAST *)(root))->args[i]);
//            }
//            ((CallExprAST *)(root))->identifier.print();
//            break;
//        default:
//            break;
//        }
//    }
//}

#ifdef ARDUINO
void setup() {
    init();
    Serial.begin(9600);

}
#endif

int main() {
#ifndef ARDUINO
    string str;
    cout << "Ready: ";
    while (getline(cin, str)) {
        if (str == "exit" || str == "quit") {
            break;
        }
        if (str.empty()) {
            cout << "Ready: ";
            continue;
        }
        if (str == "clear") {
            symbol_table.clear();
            str.clear();
            cout << "Symbol table cleared" << endl;
            cout << "Ready: ";
            continue;
        }
        Lexer::load_input_string(str.c_str());
        if (Lexer::current_tok.id == Lexer::tok_end) {
            cout << "Ready: ";
            continue;
        }
        ExprAST *ast = Parser::parse_statement(NULL);

        ast_post_order_traverse(ast);

        if (s.size() != 1) {
            cout << "Syntax error" << endl;
        } else {
            Symbol val = s.pop();
            if (val.is_var) {
                StaticArray<Symbol, 16>::iterator_prototype iter = symbol_table.find(val);
                if (iter == symbol_table.end()) {
                    cout << "Undefined symbol" << endl;
                } else {
                    cout << iter->value << endl;
                }

            } else {
                cout << val.value << endl;
            }

        }
        s.clear();
        delete ast;
        cout << "Ready: ";
    }
#else
    setup();
    StaticString<128> str;
    char c;
    Serial.print("Ready: ");
    while (true) {
        if (Serial.available() > 0) {
            c = Serial.read();
            if (c == '\n' || c == '\r') {
                Serial.println();
                if (str.empty()) {
                    Serial.print("Ready: ");
                    continue;
                }
                if (str == "clear") {
                    symbol_table.clear();
                    Serial.println("Symbol table cleared");
                    str.clear();
                    Serial.print("Ready: ");
                    continue;
                }
                Lexer::load_input_string(str.c_str());
                if (Lexer::current_tok.id == Lexer::tok_end) {
                    str.clear();
                    Serial.print("Ready: ");
                    continue;
                }
                ExprAST *ast = Parser::parse_statement(NULL);
                ast_post_order_traverse(ast);
                if (s.size() != 1) {
                    Serial.println("Syntax error");
                } else {
                    Symbol val = s.pop();
                    if (val.is_var) {
                        StaticArray<Symbol, 16>::iterator_prototype iter = symbol_table.find(val);
                        if (iter == symbol_table.end()) {
                            Serial.println("Undefined symbol");
                        } else {
                            Serial.println(iter->value);
                        }
                    } else {
                        Serial.println(val.value);
                    }
                }
                // Serial.flush();
                s.clear();
                str.clear();
                delete ast;
                Serial.print("Ready: ");
            } else if (c == '\b') {
                if (str.pop())
                    Serial.print("\b \b");
            } else {
                if (str.append(c))
                    Serial.print(c);
            }
        }

    }
    Serial.end();

#endif



//    while (Lexer::current_tok.id != Lexer::tok_end) {
//        switch (Lexer::current_tok.id) {
//        case Lexer::tok_character:
//            cout << Lexer::current_tok.c << endl;
//            break;
//        case Lexer::tok_number:
//            cout << Lexer::current_tok.num << endl;
//            break;
//        case Lexer::tok_identifier:
//            Lexer::current_tok.str.print();
//            break;
//        default:
//            break;
//        }
//        Lexer::get_next_token();
//    }

    return 0;
} 
