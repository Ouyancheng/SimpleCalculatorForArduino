# SimpleCalculatorForArduino
A simple calculator for Arduino and PC 
***

This is our proposed project for CMPUT 274, based on recursive descent parser

Author: Yancheng Ou, Yun Cao

### Grammar:
```
<stat> ::= IDENTIFIER = <expr>
         | <expr>

<expr> ::= <term> <expr_tail>

<identifier_expr> ::= IDENTIFIER
                    | IDENTIFIER ( )
                    | IDENTIFIER ( <expr> )
                    | IDENTIFIER ( <expr> , <expr> )

// <identifier_expr> ::= IDENTIFIER
//                     | IDENTIFIER ( )
//                     | IDENTIFIER ( <expr>,* <expr> )


<expr_tail> ::= + <term> <expr_tail>
              | - <term> <expr_tail>
              | * <term> <expr_tail>
              | / <term> <expr_tail>
              | EMPTY

<term> ::= <positive_term>
         | - <positive_term>
         | + <positive_term> 

<positive_term> ::= <identifier_expr>
                  | NUM
                  | ( <expr> )


IDENTIFIER: {VARIABLE_NAME, sin, cos, tan, pow, abs, sqrt, max, min, log, log2, log10}
VARIABLE_NAME: [A-Za-z][A-Za-z0-9]*
NUM: [0-9.]+
```

### Usage:
Running on PC: simply run g++ main.cpp -Wall and ./a.out

Uploading to Arduino: use Arduino IDE to upload the code to Arduino platform and open the serial monitor. 

When running on PC, type exit or quit will quit the program.

On both platform, type clear will clear the symbol table.

This program supports variable assignment and some common functions.

### Limitations: 
- The maximum length of identifier name is 16 bytes (ascii characters)
- The maximum length of input string is 128 bytes (ascii characters)
- The maximum number of different variables is 16
- Passing incorrect number of parameters to a function may cause unexpected result.

### Adding New Functions:
It's very simple to add new functions. Just open ASTWalker.hpp and scroll down to the end, you will see a bunch of else-if statements. And we can write a new else-if block between the last else-if block and the else block.

For example:
We want to add a single-parameter function exp. Then we can write:
```
else if ...
else if (MATCH_FUNCTION_NAME("exp")) {
    SINGLE_ARG_FUNCTION(va);
    s.push(exp(va));
}
else ...
```
If we want to add a double-parameter function, for example an add function which returns the sum of its two arguments, we can write:
```
else if ...
else if (MATCH_FUNCTION_NAME("add")) {
    DOUBLE_ARGS_FUNCTION(va, vb);
    s.push(va + vb);
}
else ...
```
The macro MATCH_FUNCTION_NAME helps you to define the function name.

The macro SINGLE_ARG_FUNCTION and DOUBLE_ARGS_FUNCTION helps you to specify the number of arguments.

