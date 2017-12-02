#ifndef _INCLUDES_HPP_
#define _INCLUDES_HPP_

// #define ARDUINO 1
// ARDUINO already defined

#ifdef ARDUINO
#	include <Arduino.h> 
#else 
#	include <cstddef>
#	include <cstring>
#	include <iostream>
#       include <cctype>
#       include <string>
#       include <sstream>
#       include <cstdio>
#       include <cmath>
#       define isAlphaNumeric isalnum
#       define isDigit isdigit
#       define isAlpha isalpha
#       define isSpace isspace
using namespace std;
double atof(const char *const str) {
    stringstream ss;
    ss << str;
    double result;
    ss >> result;
    return result;
}

#endif 

#endif 
