#ifndef DATA_H_AXP9YE1F
#define DATA_H_AXP9YE1F

#include <map>
#include <string>

#include "mpreal.h"

#include "token.h"
#include "function.h"

//! Class for storing defined functions and variables
/*!
 *  
 */
class Data {
public:
	//! Constructor, reads in from file called 'file_name'
	/*!
	 * Passes each line in the file through the tokenizer, rpn parser, and
	 * evaluator. This will add any definitions to the object as it goes.
	 */
	Data(std::string file_name);

	Function function(std::string name) const;
	mpfr::mpreal variable(std::string name) const;

	void add_function(std::string name, Function function);
	void add_variable(std::string name, mpfr::mpreal value);

	void add_to_history(const std::vector<Token> infix_input);

	// Save to default file
	void save() const;

	// Specify a save file
	void save(std::string file_name) const;

private:
	//! Default save file
	std::string m_file_name;

	std::map<std::string, Function> m_functions;
	std::map<std::string, mpfr::mpreal> m_variables;

	std::vector<std::vector<Token>> m_history;
};

#endif /* end of include guard: DATA_H_AXP9YE1F */

