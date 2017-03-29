/*
 * Parses infix to RPN (reverse Polish notation) using Shunting Yard
 * algorithm.
 */

#include <vector>

// #define NDEBUG
#include <cassert>

#include "rpn_parser.h"
#include "token.h"
#include "const.h"
#include "error.h"

std::vector<Token> to_rpn(std::vector<Token> infix_input)
{
	std::vector<Token> output_queue;
	std::vector<Token> stack;

	int open_bracket_count = 0; int close_bracket_count = 0;

	for (std::size_t i = 0; i < infix_input.size(); ++i) {
		if (is_unary_minus(infix_input, i)) {
			infix_input[i] = Token(Const::unary_minus, 1);
		}

		const Token & token = infix_input[i];
		switch (token.type()) {
			case Const::number:
				output_queue.push_back(token);
				break;
			case Const::variable:
			{
				Token token_copy = token;
				if (infix_input.size() > i+1) {
					bool b = infix_input[i+1].type() ==
						Const::assignment_op;
					token_copy.definition(b);
				}
				output_queue.push_back(token_copy);
				break;
			}
			case Const::function:
			{
				bool definition = false;
				int num_args = get_num_args(infix_input, i, definition);
				Token token_copy = token;
				token_copy.args_required(num_args);
				token_copy.definition(definition);

				stack.push_back(token_copy);
				break;
			}
			case Const::separator:
				parse_separator(output_queue, stack);
				break;
			case Const::open_bracket:
				open_bracket_count++;
				stack.push_back(token);
				break;
			case Const::close_bracket:
				close_bracket_count++;
				parse_close_bracket(output_queue, stack);
				break;
			default:
				parse_operator(token, output_queue, stack);
		}
	}

	if (open_bracket_count != close_bracket_count) {
		throw Error("mismatched parentheses");
	}

	for (std::size_t i = 1; i <= stack.size(); ++i) {
		output_queue.push_back(stack[stack.size()-i]);
	}

	return output_queue;
}

bool is_unary_minus(const std::vector<Token> & infix_input, std::size_t pos)
{
	Token minus = infix_input[pos];
	if (minus.type() != '-') return false;

	if (pos == 0) return true;

	Token previous = infix_input[pos-1];
	if (previous.type() == Const::number) return false;
	if (previous.type() == Const::variable) return false;
	if (previous.type() == '!') return false;
	if (previous.type() == Const::close_bracket) return false;

	// Otherwise it is unary (i.e. if it follows a normal operator or opening
	// bracket).
	return true;
}

void parse_separator(std::vector<Token> & output_queue,
                     std::vector<Token> & stack)
{
	while (true) {
		if (stack.empty())
			throw Error("format error");

		if (stack.back().type() == Const::open_bracket) {
			break;
		}
		output_queue.push_back(stack.back());
		stack.pop_back();
	}
}

void parse_close_bracket(std::vector<Token> & output_queue,
                         std::vector<Token> & stack)
{
	if (stack.empty())
		throw Error("mismatched parentheses");

	while (stack.back().type() != Const::open_bracket) {
		output_queue.push_back(stack.back());
		stack.pop_back();
		if (stack.empty())
			throw Error("mismatched parentheses");
	}
	stack.pop_back();  // Pop the open bracket

	if (!stack.empty()) {
		Token token(stack.back());
		if (token.type() == Const::function) {
			output_queue.push_back(token);
			stack.pop_back();
		}
	}
}

void parse_operator(const Token & op,
                    std::vector<Token> & output_queue,
                    std::vector<Token> & stack)
{
	if (stack.empty()) {
		stack.push_back(op);
		return;
	}

	int p1 = precedence(op);
	int p2 = precedence(stack.back());
	while ((is_left_associative(op) && p1 <= p2) || (p1 < p2)) {
		output_queue.push_back(stack.back());
		stack.pop_back();

		if (stack.empty()) break;
		else p2 = precedence(stack.back());
	}

	stack.push_back(op);

}

int get_num_args(const std::vector<Token> & infix_input,
                 std::size_t position,
                 bool & definition)
{
	if (infix_input[position+1].type() != Const::open_bracket) {
		throw Error("no bracket after function");
	}

	int open_bracket_count = 1;
	int parameter_count = 0;
	std::size_t i = position + 2;
	while (open_bracket_count > 0) {
		if (infix_input.size() <= i) {
			throw Error("mismatched parentheses");
		}

		if (infix_input[i].type() == Const::open_bracket) {
			open_bracket_count++;
		} else if (infix_input[i].type() == Const::close_bracket) {
			open_bracket_count--;
		} else if (infix_input[i].type() == Const::separator &&
		           open_bracket_count == 1) {
			parameter_count++;
		}
		i++;
	}
	parameter_count++;
	if (infix_input[position+2].type() == Const::close_bracket) {
		parameter_count = 0;
	}
	if (!(infix_input.size() <= i)) {
		definition = (infix_input[i].type() == Const::assignment_op);
	}

	return parameter_count;
}

