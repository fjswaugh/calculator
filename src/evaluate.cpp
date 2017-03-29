#include <vector>
#include <cmath>
#include <map>
#include <algorithm>

// #define NDEBUG
#include <cassert>

#include "mpreal.h"

#include "evaluate.h"
#include "token.h"
#include "error.h"
#include "const.h"
#include "data.h"
#include "function.h"

std::vector<mpfr::mpreal> get_arguments(const Token & token, std::vector<Token> & stack)
{
	int num_arguments = token.args_required();
	if (stack.size() < (unsigned)num_arguments) {
		throw Error("not enough values in expression");
	}

	std::vector<mpfr::mpreal> arguments;
	for (int j = 0; j < num_arguments; ++j) {
		arguments.push_back(stack.back().value());
		stack.pop_back();
	}

	return arguments;
}

mpfr::mpreal evaluate_function(Function function,
                               const std::vector<mpfr::mpreal> & arguments,
                               Data & data)
{
	auto body = function.body();
	auto variable_names = function.variable_names();

	if (arguments.size() != variable_names.size())
		throw Error("incorrect number of arguments passed to function");

	for (std::size_t i = 0; i < variable_names.size(); ++i) {
		std::replace(body.begin(), body.end(),
		             Token(variable_names[i], Const::variable),
		             Token(arguments[i]));
	}

	return evaluate_rpn(body, data);
}

mpfr::mpreal modulo(mpfr::mpreal a, mpfr::mpreal b)
{
	return mpfr::mod(a, b);
}

// Need to split exponent (b) into two parts: integer and decimal part
// Then need to do a^exponent_integer*a^exponent_decimal
mpfr::mpreal power(mpfr::mpreal a, mpfr::mpreal b)
{
	return mpfr::pow(a, b);
}

mpfr::mpreal factorial(mpfr::mpreal x)
{
	return mpfr::gamma(x+1);
	return 0;
}

mpfr::mpreal evaluate_operator(Token op, std::vector<mpfr::mpreal> arguments)
{
	assert(arguments.size() == (unsigned)op.args_required());

	switch (op.type()) {
		case '+':
			return arguments[1] + arguments[0];
		case '-':
			return arguments[1] - arguments[0];
		case '%':
		{
			return modulo(arguments[1], arguments[0]);
		}
		case '*':
			return arguments[1] * arguments[0];
		case '/':
			if (arguments[0] == 0) {
				throw Error("division by 0");
			}
			return arguments[1] / arguments[0];
		case '^':
			return power(arguments[1], arguments[0]);
		case Const::equality_op:
			return approx_equal(arguments[1], arguments[0]) ? 1 : 0;
		case '!':
			return factorial(arguments[0]);
		case Const::unary_minus:
			return 0 - arguments[0];
		default:
			assert(false);
			break;
	}
}

mpfr::mpreal evaluate_rpn(const std::vector<Token> & rpn_input, Data & data)
{
	static std::map<std::string, mpfr::mpreal> variables;
	static std::map<std::string, Function> functions;

	std::vector<std::pair<Token, int>> function_assignment_stack;
	std::vector<Token> stack;

	for (std::size_t i = 0; i < rpn_input.size(); ++i) {
		Token token = rpn_input[i];

		if (token.type() == Const::number) {
			stack.push_back(token.value());
		} else if (token.type() == Const::variable) {
			// Set the token value to be the actual value of the variable
			token.value(data.variable(token.name()));
			stack.push_back(token);

		} else if (token.type() == Const::assignment_op) {
			if (stack.size() < 2) {
				throw Error("not enough values in expression");
			}
			Token to_define = stack[stack.size()-2];
			mpfr::mpreal value = stack.back().value();
			stack.pop_back(); stack.pop_back();

			if (to_define.type() == Const::variable) {
				data.add_variable(to_define.name(), value);

			} else if (to_define.type() == Const::function) {
				assert(!function_assignment_stack.empty());

				int function_pos = function_assignment_stack.back().second;
				Token function_token = function_assignment_stack.back().first;
				function_assignment_stack.pop_back();

				std::vector<Token> function_body(
						rpn_input.begin() + function_pos + 1,
						rpn_input.begin() + i);

				std::vector<std::string> variable_names;
				for (int j = 1; j <= function_token.args_required(); ++j) {
					variable_names.push_back(rpn_input[function_pos-j].name());
				}

				Function function(function_body, variable_names);
				data.add_function(function_token.name(), function);
			} else {
				throw Error("can only assign to function or variable");
			}
			stack.push_back(Token(value));
		} else {
		// Token is operator or function

			std::vector<mpfr::mpreal> arguments = get_arguments(token, stack);

			if (token.type() == Const::function) {
				if (token.definition()) {
					function_assignment_stack.push_back(
							std::pair<Token, int>{token, i});
					stack.push_back(token);
				} else {
					Function f = data.function(token.name());
					Token t = Token(evaluate_function(f, arguments, data));
					stack.push_back(t);
				}
			} else {
				Token t = Token(evaluate_operator(token, arguments));
				stack.push_back(t);
			}
		}

	}


	if (stack.size() == 1) {
		return stack.back().value();
	}

	if (stack.size() > 1) {
		throw Error("too many values in expression");
	}

	return 0;

}

