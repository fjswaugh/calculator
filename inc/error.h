#ifndef ERROR_H_JYCUHP5A
#define ERROR_H_JYCUHP5A

#include <stdexcept>
#include <string>

class Error : public std::runtime_error {
public:
	Error(const std::string & message)
		: std::runtime_error(message)
	{}
};

#endif /* end of include guard: ERROR_H_JYCUHP5A */
