#ifndef CALCULATE_H_VPCTNXSY
#define CALCULATE_H_VPCTNXSY

#include <string>

#include "mpreal.h"

#include "data.h"

std::string interpret_line(std::string line, Data& data);

mpfr::mpreal calculate(std::string line, Data& data);

#endif /* end of include guard: CALCULATE_H_VPCTNXSY */

