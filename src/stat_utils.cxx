#include "stat_utils.h"

#include <cmath>
#include <optional>
#include <stdexcept>

#include "WarningLevel.h"

namespace ROOThelpers {

std::optional<int> order_of_magnitude(double val) {
  if (!std::isfinite(val)) {
    throw std::domain_error("order_of_magnitude: input must be finite");
  }
  if (val == 0.0) {
    return std::nullopt;
  }
  double log_val = std::log10(std::fabs(val));
  int oom = static_cast<int>(std::floor(log_val));
  return oom;
}

double power_of_ten(int exponent) {
  if (exponent>MAX_EXPONENT10 || exponent<MIN_EXPONENT10){
    throw std::domain_error("power_of_ten: exponent out of range for double");
  }
  constexpr int OFFSET = POWERS_OF_TEN.size() / 2;
  if (exponent >= -OFFSET && exponent <= OFFSET) {
    return POWERS_OF_TEN.at(exponent + OFFSET);
  }
  return std::pow(10.0, exponent);
}

namespace{
double round_scaled(double val, int decimals, int scale_exponent){
  if (!std::isfinite(val)) {
    throw std::domain_error("round_decimals: value must be finite");
  }
  double base = ( val * power_of_ten(scale_exponent) ) * power_of_ten(decimals-scale_exponent);
  double result = ( std::round(base)/power_of_ten(scale_exponent) ) / power_of_ten(decimals - scale_exponent);
  if (!std::isfinite(result)) {
    throw std::domain_error("round_decimals: non-finite result");
  } 
  return result;
}
}  

double round_decimals(double val, int decimals) {
  if (!std::isfinite(val)) {
    throw std::domain_error("round_decimals: value must be finite");
  }
  int decade = order_of_magnitude(val).value_or(0);
  if (decimals + decade < 0) {
    throw std::domain_error(
        "round_decimals: value of decimals may cause underflow");
  }
  if (decimals + decade > MAX_DIGITS) {
    throw std::domain_error(
        "round_decimals: decimals exceed double precision");
  }
  
  if (std::fpclassify(val) == FP_SUBNORMAL) {
    handle_warning("round_decimals: subnormal input");
    auto ulp_oom = order_of_magnitude(ULP);
    if (ulp_oom.has_value() && decimals > 1 - ulp_oom.value_or(MIN_EXPONENT10)){
      throw std::domain_error(
        "round_decimals: decimals exceed subnormal double precision");
    }
    return round_scaled(val, decimals, MAX_EXPONENT10/2);
  }

  double power = power_of_ten(decimals);
  double result = std::round(val * power)/power;

  if (!std::isfinite(result)) {
    throw std::domain_error("round_decimals: non-finite result");
  }
  return result;
}

bool equal_up_to(double val1, double val2, int decimals) {
  const double tolerance = power_of_ten(-decimals - 1);
  const double diff =
      round_decimals(val1, decimals) - round_decimals(val2, decimals);
  return std::fabs(diff) <= tolerance;
}

} // namespace ROOThelpers