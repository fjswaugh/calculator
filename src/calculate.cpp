#include <string>
#include <vector>
#include <sstream>

#include "mpreal.h"

#include "calculate.h"
#include "error.h"
#include "const.h"
#include "token.h"
#include "evaluate.h"
#include "rpn_parser.h"
#include "data.h"

std::string interpret_line(std::string line, Data& data)
{
	std::string return_string;

	if (line == "save") {
		data.save();
		return_string = "";
	} else {
		std::ostringstream os;
		os << calculate(line, data);
		os << '\n';

		return_string = os.str();
	}

	return return_string;
}

mpfr::mpreal calculate(std::string line, Data& data)
{
	std::vector<Token> infix_input = tokenize(line);
	data.add_to_history(infix_input);

	std::vector<Token> rpn_input = to_rpn(infix_input);

	mpfr::mpreal answer = evaluate_rpn(rpn_input, data);

	data.add_variable("ans", answer);

	return answer;
}

