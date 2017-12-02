# SimpleCalculatorForArduino
A simple calculator for Arduino 
***

This is our project for CMPUT 274, based on LL(*) grammar and recursive descent parser 

Author: Yancheng Ou, Yun Cao 

BNF: 
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

<term> ::= <identifier_expr>
         | NUM
         | ( <expr> )


IDENTIFIER: {VARIABLE_NAME, sin, cos, tan, pow, abs, sqrt, max, min}
VARIABLE_NAME: [A-Za-z][A-za-z0-9]*
NUM: [0-9.]+ 
``` 

### Usage: 
Running on PC: simply run g++ main.cpp -Wall and ./a.out 
Uploading to Arduino: type make upload and open the serial monitor. 

When running on PC, type exit or quit will quit the program. 
On both platform, type clear will clear the symbol table. 

This program supports variable assignment and some common functions. 
But the maximum length of identifier name is 16 bytes, maximum length of input string is 128 bytes and maximum number of different variables is 16. 

### Limitations: 
Note that this program doesn't handle the negative operator (unary). A negative number before a number literal will be treated as part of the number. 
But expressions like -a, -sin(3.14) are not supported. 
And passing only one parameter to a two-parameter function (such as max, min, pow) may cause unexpected result. 

