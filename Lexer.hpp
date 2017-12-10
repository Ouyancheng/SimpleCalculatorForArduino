#ifndef _LEXER_HPP_
#define _LEXER_HPP_
#include "includes.hpp"
#include "StaticString.hpp"
namespace Lexer {
enum TokenId {
    tok_number,
    tok_identifier,
    tok_character,
    tok_def,
    tok_end,
    tok_empty
};

struct Token {
    TokenId id;

    double num;
    char c;
    StaticString<16> str;

    Token() : id(tok_empty) {}
    Token(double num) : id(tok_number), num(num), c('\0') {}
    Token(char c) : id(tok_character), num(0.0), c(c) {}
    template <size_t Capacity>
    Token(const StaticString<Capacity> &str) : id(tok_identifier), num(0.0), c('\0'), str(str) {}

};

Token current_tok;

StaticString<128> str;
size_t pos;

void get_next_token();
void load_input_string(const char *const input_str) {
    str = input_str;
    pos = 0;
    current_tok.id = tok_empty;
    get_next_token(); // Load the first token
}

void get_next_token() {
    if (pos >= str.size()) {
        current_tok.id = tok_end;
        return;
    }
    StaticString<16> temp_str;

    while (pos <str.size() && isSpace(str[pos])) ++pos; // skip whitespaces
    if (pos >= str.size()) {
        current_tok.id = tok_end;
        return;
    }

    if (isDigit(str[pos]) ||
            (current_tok.id != tok_number && current_tok.id != tok_identifier &&
             !(current_tok.id == tok_character && current_tok.c == ')') &&
             str[pos] == '-')) { // NUM
        temp_str.append(str[pos++]);
        while (pos < str.size() && (isDigit(str[pos]) || str[pos] == '.')) {
            temp_str.append(str[pos]);
            ++pos;
        }
        current_tok = Token(temp_str.to_double());

    } else if (isAlpha(str[pos])) { // ID
        temp_str.append(str[pos++]);
        while (pos < str.size() && (isAlphaNumeric(str[pos]))) {
            temp_str.append(str[pos]);
            ++pos;
        }

        current_tok = Token(temp_str);
        if (temp_str == "def") {
            current_tok.id = tok_def;
        }
    } else if (str[pos] == '\0') {
        current_tok.id = tok_end;
    } else {
        current_tok = Token(str[pos]);
        ++pos;
    }
}

Token peek_next_token(int how_many = 1) {
    if (how_many <= 0) return Token();
    Token backup_tok(current_tok);
    size_t pos_backup = pos;
    for (int i = 0; i < how_many; i++) {
        get_next_token();
    }
    Token result_tok = current_tok;
    current_tok = backup_tok;
    pos = pos_backup;
    return result_tok;
}

}

#endif // ! _LEXER_HPP_ 
