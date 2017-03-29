#ifndef EVALUATE_H_S7B3FPRI
#define EVALUATE_H_S7B3FPRI

#include <string>
#include <vector>

#include "mpreal.h"

#include "token.h"
#include "function.h"
#include "data.h"

std::vector<mpfr::mpreal> get_arguments(const Token & token, std::vector<Token> & stack);

mpfr::mpreal evaluate_rpn(const std::vector<Token> & rpn_input, Data & data);

mpfr::mpreal evaluate_function(Function function,
                         const std::vector<mpfr::mpreal> & arguments,
                         Data & data);

mpfr::mpreal modulo(mpfr::mpreal a, mpfr::mpreal b);

mpfr::mpreal power(mpfr::mpreal a, mpfr::mpreal b);

mpfr::mpreal factorial(mpfr::mpreal x);

mpfr::mpreal evaluate_operator(Token op, std::vector<mpfr::mpreal> arguments);

#endif /* end of include guard: EVALUATE_H_S7B3FPRI */

