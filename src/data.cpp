#include <string>
#include <fstream>
#include <stdexcept>
#include <iostream>
#include <algorithm>

#include "mpreal.h"

#include "const.h"
#include "data.h"
#include "token.h"
#include "function.h"
#include "error.h"
#include "evaluate.h"
#include "rpn_parser.h"
#include "calculate.h"

Data::Data(std::string file_name)
	: m_file_name(file_name)
{
	std::ifstream file;
	file.open(file_name.c_str());

	std::string line;
	while (std::getline(file, line))
	try {
		calculate(line, *this);
	}
	catch (const Error & e) {
		std::cerr << "Error: " << e.what() << std::endl;
	}
}

Function Data::function(std::string name) const
try {
	return m_functions.at(name);
}
catch(...) {
	// Function not yet defined, return empty one
	return Function();
}

mpfr::mpreal Data::variable(std::string name) const
try {
	return m_variables.at(name);
}
catch(...) {
	// Variable not yet defined, return 0
	return 0;
}

void Data::add_function(std::string name, Function function)
{
	m_functions[name] = function;
}

void Data::add_variable(std::string name, mpfr::mpreal value)
{
	m_variables[name] = value;
}

void Data::add_to_history(const std::vector<Token> infix_input)
{
	m_history.push_back(infix_input);
}

void Data::save() const
{
	save(m_file_name);
}

void Data::save(std::string file_name) const
{
	std::ofstream file;
	file.open(file_name.c_str());

	for (auto i : m_history) {
		if (std::find(i.begin(), i.end(), Token(Const::assignment_op, 0)) !=
				i.end()) {
			for (auto j : i) {
				file << j << ' ';
			}
			file << std::endl;
		}
	}
}

