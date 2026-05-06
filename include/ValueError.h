#ifndef VALUEERROR_H
#define VALUEERROR_H

#include <iostream>

namespace ROOThelpers {
constexpr int DEFAULT_SIGNIFICANT_DIGITS = 2;

class ValueError {
private:
  double value_;
  double lerror_;
  double rerror_;
  int significant_digits_ = DEFAULT_SIGNIFICANT_DIGITS;

  struct RoundSpec {
    int decimals;
    int precision;
  };

  [[nodiscard]] double getRefError() const;
  [[nodiscard]] RoundSpec getRoundSpec() const;

public:
  ValueError(double val, double err);
  ValueError(double val, double lerr, double rerr);

  [[nodiscard]] double getValue() const;
  [[nodiscard]] double getLError() const;
  [[nodiscard]] double getRError() const;
  [[nodiscard]] bool isSymmetric() const;
  [[nodiscard]] int getSignificantDigits() const;

  void setSignificantDigits(int n_dig);
  bool operator==(const ValueError &other) const;
  [[nodiscard]] bool isConsistentWith(const ValueError &other,
                                      double sigmas = 1.0) const;

  friend std::ostream &operator<<(std::ostream &ost, const ValueError &verr);
};

} // namespace ROOThelpers
#endif