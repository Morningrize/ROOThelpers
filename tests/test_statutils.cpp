
#include <cmath>
#include <iostream>
#include <limits>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>

#include <gtest/gtest.h>

#include "WarningLevel.h"
#include "stat_utils.h"

const double SUBNORMAL_MIN = std::numeric_limits<double>::denorm_min();
const double SUBNORMAL_MAX =
    std::nextafter(std::numeric_limits<double>::min(), 0.);
const double SUBNORMAL = sqrt(SUBNORMAL_MAX) * sqrt(SUBNORMAL_MIN);

class StatUtilsWarningTest : public ::testing::Test {
protected:
  void SetUp() override {
    ROOThelpers::set_warning_level(ROOThelpers::WarningLevel::WARN);
    old_clog_buf = std::clog.rdbuf(test_stream.rdbuf());
  }

  void TearDown() override {
    std::clog.rdbuf(old_clog_buf);
    ROOThelpers::set_warning_level(ROOThelpers::WarningLevel::WARN);
    test_stream.str("");
    test_stream.clear();
  }
  std::ostringstream test_stream;         // NOLINT
  std::streambuf *old_clog_buf = nullptr; // NOLINT
};

TEST(OrderOfMagnitudeTest, UndefinedForZero) {
  EXPECT_EQ(ROOThelpers::order_of_magnitude(0.0), std::nullopt);
  EXPECT_EQ(ROOThelpers::order_of_magnitude(-0.0), std::nullopt);
}

TEST(OrderOfMagnitudeTest, ThrowsNonFiniteArgument) {
  EXPECT_THROW(
      ROOThelpers::order_of_magnitude(std::numeric_limits<double>::quiet_NaN()),
      std::domain_error);
  EXPECT_THROW(
      ROOThelpers::order_of_magnitude(std::numeric_limits<double>::infinity()),
      std::domain_error);
  EXPECT_THROW(
      ROOThelpers::order_of_magnitude(-std::numeric_limits<double>::infinity()),
      std::domain_error);
}

TEST(OrderOfMagnitudeTest, CorrectForNumbers) {
  EXPECT_EQ(ROOThelpers::order_of_magnitude(1.0), std::optional<int>{0});
  EXPECT_EQ(ROOThelpers::order_of_magnitude(10.0), std::optional<int>{1});
  EXPECT_EQ(ROOThelpers::order_of_magnitude(0.1), std::optional<int>{-1});
  EXPECT_EQ(ROOThelpers::order_of_magnitude(-0.1), std::optional<int>{-1});
}

TEST(OrderOfMagnitudeTest, ExtremeMagnitudes) {
  EXPECT_EQ(ROOThelpers::order_of_magnitude(1.e+100), std::optional<int>{100});
  EXPECT_EQ(ROOThelpers::order_of_magnitude(1.e-100), std::optional<int>{-100});
}

TEST(OrderOfMagnitudeTest, SubnormalInput) {
  EXPECT_TRUE(
      ROOThelpers::order_of_magnitude(std::numeric_limits<double>::denorm_min())
          .has_value());
}

TEST(PowerOfTenTest, ConsistencyWithPow) {
  for (int exp = ROOThelpers::MIN_EXPONENT10;
       exp <= ROOThelpers::MAX_EXPONENT10; exp++) {
    EXPECT_DOUBLE_EQ(ROOThelpers::power_of_ten(exp), std::pow(10.0, exp));
  }
}

TEST(PowerOfTenTest, OverflowUnderflow) {
  EXPECT_TRUE(
      std::isfinite(ROOThelpers::power_of_ten(ROOThelpers::MAX_EXPONENT10)));
  EXPECT_TRUE(
      std::isfinite(ROOThelpers::power_of_ten(ROOThelpers::MIN_EXPONENT10)));
  EXPECT_THROW(ROOThelpers::power_of_ten(ROOThelpers::MAX_EXPONENT10 + 1),
               std::domain_error);
  EXPECT_THROW(ROOThelpers::power_of_ten(ROOThelpers::MIN_EXPONENT10 - 1),
               std::domain_error);
}

TEST(RoundDecimalsTest, ThrowsNonFiniteArgument) {
  EXPECT_THROW(
      ROOThelpers::round_decimals(std::numeric_limits<double>::quiet_NaN(), 1),
      std::domain_error);
  EXPECT_THROW(
      ROOThelpers::round_decimals(std::numeric_limits<double>::infinity(), 1),
      std::domain_error);
  EXPECT_THROW(
      ROOThelpers::round_decimals(-std::numeric_limits<double>::infinity(), 1),
      std::domain_error);
}

TEST(RoundDecimalsTest, ThrowsOnUnderflow) {
  EXPECT_THROW(ROOThelpers::round_decimals(0.1, 0), std::domain_error);
  EXPECT_THROW(ROOThelpers::round_decimals(10.0, -2), std::domain_error);
}

TEST(RoundDecimalsTest, ThrowsOnPrecisionLoss) {
  EXPECT_THROW(ROOThelpers::round_decimals(10.0, ROOThelpers::MAX_DIGITS),
               std::domain_error);
}

TEST(RoundDecimalsTest, HandlesZero) {
  EXPECT_DOUBLE_EQ(ROOThelpers::round_decimals(0.0, 1), 0.0);
  EXPECT_DOUBLE_EQ(ROOThelpers::round_decimals(0.0, ROOThelpers::MAX_DIGITS),
                   0.0);
}

TEST_F(StatUtilsWarningTest, RoundDecimalsHandleWarningSubnormal) {
  EXPECT_NO_THROW(ROOThelpers::round_decimals(SUBNORMAL_MAX,
                                              1 - ROOThelpers::MIN_EXPONENT10));
  EXPECT_NE(test_stream.str().find("round_decimals: subnormal input"),
            std::string::npos);
}

TEST(RoundDecimalsTest, RoundToIntegers) {
  EXPECT_DOUBLE_EQ(ROOThelpers::round_decimals(1.1, 0), std::round(1.1));
  EXPECT_DOUBLE_EQ(ROOThelpers::round_decimals(1.5, 0), std::round(1.5));
  EXPECT_DOUBLE_EQ(ROOThelpers::round_decimals(-1.5, 0), std::round(-1.5));
}
