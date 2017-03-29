#ifndef TOKEN_H_RKWGOAGX
#define TOKEN_H_RKWGOAGX

#include <string>
#include <vector>
#include <map>

#include "mpreal.h"

class Token {
public:
	Token();
	Token(char ch, int args);
	Token(mpfr::mpreal val);
	Token(std::string name, char type);

	char type() const { return m_type; }
	mpfr::mpreal value() const { return m_value; }
	int args_required() const { return m_args_required; }
	std::string name() const { return m_name; }
	bool definition() const { return m_definition; }

	void value(mpfr::mpreal value);
	void definition(bool value);

	void args_required(int num);

private:
	char m_type;
	mpfr::mpreal m_value;
	int m_args_required;
	std::string m_name;  // For variables
	bool m_definition;  // True if the variable/function is being defined
};

// Helper functions

std::vector<Token> tokenize(const std::string & line);

bool operator==(const Token & a, const Token & b);
bool operator!=(const Token & a, const Token & b);

std::ostream & operator<<(std::ostream & os, const Token & token);

std::istream & operator>>(std::istream & is, Token & token);

bool is_left_associative(Token token);

int precedence(Token token);

// General functions

bool approx_equal(mpfr::mpreal a, mpfr::mpreal b);

#endif /* end of include guard: TOKEN_H_RKWGOAGX */

