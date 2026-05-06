#include "ValueError.h"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <stdexcept>

#include "WarningLevel.h"
#include "stat_utils.h"
#include "StreamStateGuard.h"

namespace ROOThelpers {

ValueError::ValueError(double val, double err) : ValueError(val, err, err) {}

ValueError::ValueError(double val, double lerr, double rerr)
    : value_(val), lerror_(lerr), rerror_(rerr) {
  if (!std::isfinite(val)) {
    throw std::invalid_argument("ValueError: value must be finite");
  }
  if (!std::isfinite(rerr) || !std::isfinite(lerr)) {
    throw std::invalid_argument("ValueError: errors must be finite");
  }
  if (lerr < 0 || rerr < 0) {
    throw std::invalid_argument("ValueError: errors must be non-negative");
  }
  if ((std::fpclassify(val) == FP_SUBNORMAL) ||
      (std::fpclassify(lerr) == FP_SUBNORMAL) ||
      (std::fpclassify(rerr) == FP_SUBNORMAL)) {
    ROOThelpers::handle_warning(
        "ValueError: constructed using subnormal input");
  }
}

double ValueError::getValue() const { return value_; }
double ValueError::getLError() const { return lerror_; }
double ValueError::getRError() const { return rerror_; }
bool ValueError::isSymmetric() const { return lerror_ == rerror_; }
int ValueError::getSignificantDigits() const { return significant_digits_; }

bool ValueError::operator==(const ValueError &other) const {
  const auto spec_this = getRoundSpec();
  const auto spec_other = other.getRoundSpec();
  int decimals = std::min(spec_this.decimals, spec_other.decimals);

  return equal_up_to(value_, other.value_, decimals) &&
         equal_up_to(lerror_, other.lerror_, decimals) &&
         equal_up_to(rerror_, other.rerror_, decimals);
}

bool ValueError::isConsistentWith(const ValueError& other, double sigmas) const {
  if (!std::isfinite(sigmas) || sigmas < 0.0){
    throw std::invalid_argument("ValueError::isConsistentWith: sigmas must be finite and non-negative");
  }
  const double low_this = value_ - ( sigmas * lerror_ );
  const double high_this = value_ + ( sigmas * rerror_ );
  const double low_other = other.value_ - ( sigmas * other.lerror_ );
  const double high_other = other.value_ + ( sigmas * other.rerror_ );

  return std::max(low_this,low_other) <= std::min(high_this, high_other);
}

void ValueError::setSignificantDigits(int n_dig) {
  if (n_dig < 1) {
    throw std::invalid_argument(
        "ValueError: significant digits must be at least 1");
  }
  if (n_dig > MAX_DIGITS) {
    throw std::invalid_argument("ValueError: too many significant digits");
  }
  significant_digits_ = n_dig;
}

double ValueError::getRefError() const {
  if (lerror_>0 && rerror_>0) {
    return std::min(lerror_,rerror_);
  }
  return std::max(lerror_,rerror_);
}

ValueError::RoundSpec ValueError::getRoundSpec() const {
  int decade = order_of_magnitude(getRefError()).value_or(0);
  int decimals = significant_digits_ - 1 - decade;
  int precision = std::max(0, decimals);
  return {.decimals = decimals, .precision = precision};
}

std::ostream &operator<<(std::ostream &ost, const ValueError &verr) {
  StreamStateGuard guard(ost);
  const auto spec = verr.getRoundSpec();

  if (verr.isSymmetric()) {
    ost << std::fixed << std::setprecision(spec.precision)
        << round_decimals(verr.getValue(), spec.decimals) << " ± "
        << round_decimals(verr.getLError(), spec.decimals);
  } else {
    ost << std::fixed << std::setprecision(spec.precision)
        << round_decimals(verr.getValue(), spec.decimals) << " - "
        << round_decimals(verr.getLError(), spec.decimals) << " + "
        << round_decimals(verr.getRError(), spec.decimals);
  }
  return ost;
}

} // namespace ROOThelpers