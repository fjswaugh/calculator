#ifndef RPN_PARSER_H_NAPIT3BR
#define RPN_PARSER_H_NAPIT3BR

#include <vector>

#include "token.h"

std::vector<Token> to_rpn(std::vector<Token> infix_input);

bool is_unary_minus(const std::vector<Token> & infix_input, std::size_t pos);

// Separator is just a comma in a list of arguments
void parse_separator(std::vector<Token> & output_queue,
                     std::vector<Token> & stack);

void parse_open_bracket(std::vector<Token> & output_queue,
                        std::vector<Token> & stack);

void parse_close_bracket(std::vector<Token> & output_queue,
                         std::vector<Token> & stack);

void parse_operator(const Token & op,
                    std::vector<Token> & output_queue,
                    std::vector<Token> & stack);

int get_num_args(const std::vector<Token> & infix_input,
                 std::size_t position,
                 bool & definition);

#endif /* end of include guard: RPN_PARSER_H_NAPIT3BR */

