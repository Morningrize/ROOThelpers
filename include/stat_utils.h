#ifndef STATUTILS_H
#define STATUTILS_H

#include <array>
#include <limits>
#include <optional>

namespace ROOThelpers {

constexpr int MAX_DIGITS = std::numeric_limits<double>::max_digits10;
constexpr int MAX_EXPONENT10 = std::numeric_limits<double>::max_exponent10;
constexpr int MIN_EXPONENT10 = std::numeric_limits<double>::min_exponent10;
const double ULP = std::numeric_limits<double>::denorm_min();

constexpr std::array<double, 31> POWERS_OF_TEN = {
    1e-15, 1e-14, 1e-13, 1e-12, 1e-11, 1e-10, 1e-9, 1e-8, 1e-7, 1e-6, 1e-5,
    1e-4,  1e-3,  1e-2,  1e-1,  1e0,   1e1,   1e2,  1e3,  1e4,  1e5,  1e6,
    1e7,   1e8,   1e9,   1e10,  1e11,  1e12,  1e13, 1e14, 1e15};

std::optional<int> order_of_magnitude(double val);

double power_of_ten(int exponent);

double round_decimals(double val, int decimals);

bool equal_up_to(double val1, double val2, int decimals);

} // namespace ROOThelpers
#endif