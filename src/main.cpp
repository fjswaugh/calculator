#include <vector>
#include <iostream>
#include <string>
#include <istream>
#include <stdexcept>

#include <cstdlib>
#include <unistd.h>

// #define NDEBUG  // Uncomment to disable asserts
#include <cassert>

#include "mpreal.h"

#include "error.h"
#include "const.h"
#include "data.h"
#include "calculate.h"

void prompt(Data& data, bool piped)
try {
	if (!piped) std::cout << "> ";
	std::string line;
	if (!std::getline(std::cin, line)) return;

	if (line == "exit" || line == "quit") {
		return;
	} else {
		std::cout << interpret_line(line, data);
	}

	prompt(data, piped);
}
catch (const Error & e) {
	std::cerr << "Error: " << e.what() << std::endl;
	prompt(data, piped);
}

int main()
try {
	const int digits = 1000;
	mpfr::mpreal::set_default_prec(mpfr::digits2bits(digits));
	std::cout.precision(digits);

	bool piped = true;
	if (isatty(STDIN_FILENO)) {
		piped = false;
	}

	std::string default_load_location = std::getenv("HOME");
	default_load_location += "/.calcrc";
	Data data(default_load_location);

	if (!piped) {
		std::cout << "Calculator version " << Const::version << "\n";
		std::cout << "Enter a calculation at the prompt:\n\n";
	}
    prompt(data, piped);
	if (!piped) {
		std::cout << std::endl;
	}
}
catch (const std::runtime_error & e) {
	std::cerr << "Error: " << e.what() << std::endl;
	return 1;
}
catch (...) {
	std::cerr << "Error: unknown" << std::endl;
	return 2;
}

