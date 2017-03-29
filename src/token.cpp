#include <string>
#include <istream>
#include <ostream>
#include <sstream>

// #define NDEBUG  // Uncomment to disable asserts
#include <cassert>

#include "mpreal.h"

#include "token.h"
#include "const.h"
#include "error.h"

Token::Token()
	: m_type(Const::uninitialized), m_value(0), m_args_required(0),
	  m_definition(false)
{}

Token::Token(char ch, int args)
	: m_type(ch), m_value(0), m_args_required(args),
	  m_definition(false)
{}

Token::Token(mpfr::mpreal val)
	: m_type(Const::number), m_value(val), m_args_required(0),
	  m_definition(false)
{}

Token::Token(std::string name, char type)
	: m_type(type), m_value(0), m_args_required(0), m_name(name),
	  m_definition(false)
{
	assert(m_type == Const::variable || m_type == Const::function);
}

void Token::value(mpfr::mpreal value)
{
	m_value = value;
}

void Token::definition(bool value)
{
	m_definition = value;
}

void Token::args_required(int num)
{
	m_args_required = num;
}

// Helper functions

std::vector<Token> tokenize(const std::string & line)
{
	std::vector<Token> output;
	std::istringstream input(line);

	Token token;
	while (input >> token)
		output.push_back(token);

	if (input.fail() && !input.bad() && !input.eof()) {
		input.clear(std::ios_base::goodbit);
		input.unget();

		char bad_char;
		if (!(input >> bad_char))
			throw Error("problem with input stream");

		std::string error_message("bad token - ");
		error_message += bad_char;
		throw Error(error_message);
	}

	if (input.bad() && !input.eof())
		throw Error("problem with input stream");

	return output;
}

bool operator==(const Token & a, const Token & b)
{
	if (a.type() != b.type()) return false;
	if (!approx_equal(a.value(), b.value())) return false;
	if (a.name() != b.name()) return false;
	if (a.args_required() != b.args_required()) return false;
	if (a.definition() != b.definition()) return false;

	return true;
}

bool operator!=(const Token & a, const Token & b)
{
	return !(a == b);
}

std::ostream & operator<<(std::ostream & os, const Token & token)
{
	if (token.type() == Const::number) {
		return os << token.value();
	} else if (token.type() == Const::variable ||
	           token.type() == Const::function) {
		return os << token.name();
	} else if (token.type() == Const::uninitialized) {
		return os;
	} else if (token.type() == Const::unary_minus) {
		return os << "_";
	} else if (token.type() == Const::equality_op) {
		return os << "==";
	} else if (token.type() == Const::open_bracket) {
		return os << "(";
	} else if (token.type() == Const::close_bracket) {
		return os << ")";
	} else if (token.type() == Const::separator) {
		return os << ",";
	} else if (token.type() == Const::assignment_op) {
		return os << "=";
	} else {
		return os << token.type();
	}
}

std::istream & operator>>(std::istream & is, Token & token)
{
	char ch;
	if (!(is >> ch)) return is;

	switch (ch) {
		case '(':
			token = Token(Const::open_bracket, 0);
			break;
		case ')':
			token = Token(Const::close_bracket, 0);
			break;
		case ',':
			token = Token(Const::separator, 0);
			break;
		case '!':
			token = Token(ch, 1);
			break;
		case '^':
		case '%':
		case '*': case '/':
		case '+': case '-':
			token = Token(ch, 2);
			break;
		case '.':
		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
		{
			is.putback(ch);
			mpfr::mpreal num;
			if (!(is >> num)) return is;
			token = Token(num);
		}
			break;
		case '=':
			token = Token(Const::assignment_op, 0);

			char temp;

			if (!(is >> temp)) {
				// Pretend nothing happened, and return to indicate
				// that a token was successfully read (which it was).
				// If another token is attempted to be read it will
				// fail before even reaching the switch statement.
				is.clear(std::ios_base::goodbit);
				return is;
			}

			if (temp == '=')
				token = Token(Const::equality_op, 2);
			else
				is.putback(temp);

			break;
		default:
		{
			if (std::isalpha(ch)) {
				std::string name;
				bool finished_name = false;
				while (!finished_name) {
					name.push_back(ch);
					token = Token(name, Const::variable);

					if (!is.get(ch)) {
						is.clear(std::ios_base::goodbit);
						return is;
					} else if (ch == ' ') {
						finished_name = true;
					}
					is.putback(ch);

					if (!(is >> ch)) {
						is.clear(std::ios_base::goodbit);
						return is;
					} else if (!std::isalpha(ch)) {
						finished_name = true;
						if (ch == '(') token = Token(name, Const::function);
					}

					if (finished_name) {
						is.putback(ch);
					}
					// Else ch is a continuation of name, so continue...
				}
				break;
			} else {
				is.clear(std::ios_base::failbit);
				return is;
			}
		}
	}

	return is;
}

bool is_left_associative(Token token)
{
	if (token.type() == '^') return false;
	return true;
}

int precedence(Token token)
{
	char type = token.type();
	switch (type) {
		case '!':
			return 60;
		case '^':
			return 50;
		case '*': case '/': case '%':
			return 40;
		case Const::unary_minus:
			return 35;
		case '+': case '-':
			return 30;
		case Const::equality_op:
			return 10;
		case '=':
			return 5;
		default:
			return 0;
	}
}

// General functions

bool approx_equal(mpfr::mpreal a, mpfr::mpreal b)
{
	return (a - b < Const::uncertainty &&
	        b - a < Const::uncertainty);
}

