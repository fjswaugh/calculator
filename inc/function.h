#ifndef FUNCTION_H_N29YDG8Z
#define FUNCTION_H_N29YDG8Z

#include <vector>

#include "token.h"

class Function {
public:
	Function()
		: m_body(), m_variable_names()
	{}

	Function(const std::vector<Token> & body,
	         const std::vector<std::string> & variable_names)
		: m_body(body), m_variable_names(variable_names)
	{}

	int args_required() const
	{
		return m_variable_names.size();
	}

	std::vector<Token> body() const
	{
		return m_body;
	}

	std::vector<std::string> variable_names() const
	{
		return m_variable_names;
	}

private:
	std::vector<Token> m_body;
	std::vector<std::string> m_variable_names;
};

#endif /* end of include guard: FUNCTION_H_N29YDG8Z */

