#include <cmath>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>

#include <gtest/gtest.h>

#include "ValueError.h"
#include "WarningLevel.h"

namespace {

class ValueErrorWarningTest : public ::testing::Test {
protected:
  void SetUp() override {
    ROOThelpers::set_warning_level(ROOThelpers::WarningLevel::WARN);
    old_clog_buf_ = std::clog.rdbuf(test_stream_.rdbuf());
  }

  void TearDown() override {
    std::clog.rdbuf(old_clog_buf_);
    ROOThelpers::set_warning_level(ROOThelpers::WarningLevel::WARN);
    test_stream_.str("");
    test_stream_.clear();
  }

  std::ostringstream test_stream_;         // NOLINT
  std::streambuf *old_clog_buf_ = nullptr; // NOLINT
};

} // namespace

TEST(ValueErrorTest, SymmetricConstructorStoresValues) {
  const ROOThelpers::ValueError value_error(12.3, 0.4);

  EXPECT_DOUBLE_EQ(value_error.getValue(), 12.3);
  EXPECT_DOUBLE_EQ(value_error.getLError(), 0.4);
  EXPECT_DOUBLE_EQ(value_error.getRError(), 0.4);
  EXPECT_TRUE(value_error.isSymmetric());
  EXPECT_EQ(value_error.getSignificantDigits(),
            ROOThelpers::DEFAULT_SIGNIFICANT_DIGITS);
}

TEST(ValueErrorTest, AsymmetricConstructorStoresValues) {
  const ROOThelpers::ValueError value_error(12.3, 0.1, 0.2);

  EXPECT_DOUBLE_EQ(value_error.getValue(), 12.3);
  EXPECT_DOUBLE_EQ(value_error.getLError(), 0.1);
  EXPECT_DOUBLE_EQ(value_error.getRError(), 0.2);
  EXPECT_FALSE(value_error.isSymmetric());
}

TEST(ValueErrorTest, ConstructorRejectsNonFiniteInputs) {
  EXPECT_THROW(ROOThelpers::ValueError(std::numeric_limits<double>::quiet_NaN(),
                                       0.1, 0.2),
               std::invalid_argument);
  EXPECT_THROW(ROOThelpers::ValueError(std::numeric_limits<double>::infinity(),
                                       0.1, 0.2),
               std::invalid_argument);
  EXPECT_THROW(ROOThelpers::ValueError(
                   1.0, std::numeric_limits<double>::quiet_NaN(), 0.2),
               std::invalid_argument);
  EXPECT_THROW(ROOThelpers::ValueError(1.0, 0.1,
                                       std::numeric_limits<double>::infinity()),
               std::invalid_argument);
}

TEST(ValueErrorTest, ConstructorRejectsNegativeErrors) {
  EXPECT_THROW(ROOThelpers::ValueError(1.0, -0.1, 0.2), std::invalid_argument);
  EXPECT_THROW(ROOThelpers::ValueError(1.0, 0.1, -0.2), std::invalid_argument);
}

TEST(ValueErrorTest, SetSignificantDigitsValidatesRange) {
  ROOThelpers::ValueError value_error(1.23, 0.11);

  EXPECT_THROW(value_error.setSignificantDigits(0), std::invalid_argument);
  EXPECT_THROW(value_error.setSignificantDigits(1000), std::invalid_argument);

  EXPECT_NO_THROW(value_error.setSignificantDigits(4));
  EXPECT_EQ(value_error.getSignificantDigits(), 4);
}

TEST(ValueErrorTest, EqualityUsesRoundedComparison) {
  const ROOThelpers::ValueError lhs(1.2345, 0.1234);
  const ROOThelpers::ValueError rhs_close(1.2346, 0.1235);
  const ROOThelpers::ValueError rhs_far(1.2399, 0.1299);

  EXPECT_TRUE(lhs == rhs_close);
  EXPECT_FALSE(lhs == rhs_far);
}

TEST(ValueErrorTest, IsConsistentWithChecksIntervals) {
  const ROOThelpers::ValueError center(10.0, 0.5);
  const ROOThelpers::ValueError overlapping(10.7, 0.3);
  const ROOThelpers::ValueError disjoint(12.0, 0.2);

  EXPECT_TRUE(center.isConsistentWith(overlapping));
  EXPECT_FALSE(center.isConsistentWith(disjoint));
  EXPECT_TRUE(center.isConsistentWith(disjoint, 10.0));
}

TEST(ValueErrorTest, IsConsistentWithRejectsInvalidSigmas) {
  const ROOThelpers::ValueError value_error(1.0, 0.1);
  const ROOThelpers::ValueError other(1.0, 0.1);

  EXPECT_THROW(static_cast<void>(value_error.isConsistentWith(other, -1.0)),
               std::invalid_argument);
  EXPECT_THROW(static_cast<void>(value_error.isConsistentWith(
                   other, std::numeric_limits<double>::quiet_NaN())),
               std::invalid_argument);
}

TEST_F(ValueErrorWarningTest, ConstructorWarnsOnSubnormalInput) {
  const double subnormal = std::numeric_limits<double>::denorm_min();

  EXPECT_NO_THROW(ROOThelpers::ValueError(subnormal, 0.1));
  EXPECT_NE(
      test_stream_.str().find("ValueError: constructed using subnormal input"),
      std::string::npos);
}

TEST(ValueErrorTest, StreamOutputUsesExpectedFormat) {
  ROOThelpers::ValueError symmetric(12.345, 0.67);
  ROOThelpers::ValueError asymmetric(12.345, 0.67, 0.89);
  symmetric.setSignificantDigits(3);
  asymmetric.setSignificantDigits(3);

  std::ostringstream sym_stream;
  std::ostringstream asym_stream;
  sym_stream << symmetric;
  asym_stream << asymmetric;

  EXPECT_NE(sym_stream.str().find("\xC2\xB1"), std::string::npos);
  EXPECT_NE(asym_stream.str().find(" - "), std::string::npos);
  EXPECT_NE(asym_stream.str().find(" + "), std::string::npos);
}

TEST(ValueErrorTest, StreamOutputDoesNotLeakCallerFormatting) {
  const ROOThelpers::ValueError value_error(12.345, 0.67);
  std::ostringstream stream;
  stream << std::scientific << std::setprecision(7);

  const auto original_flags = stream.flags();
  const auto original_precision = stream.precision();

  stream << value_error;

  EXPECT_EQ(stream.flags(), original_flags);
  EXPECT_EQ(stream.precision(), original_precision);
}