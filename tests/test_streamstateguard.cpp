#include <iomanip>
#include <ios>
#include <sstream>

#include <gtest/gtest.h>

#include "StreamStateGuard.h"

TEST(StreamStateGuardTest, RestoresFlagsAndPrecisionAfterScopeExit) {
  std::ostringstream stream;
  stream << std::scientific << std::setprecision(7);

  const auto original_flags = stream.flags();
  const auto original_precision = stream.precision();

  {
    const ROOThelpers::StreamStateGuard guard(stream);
    stream << std::fixed << std::setprecision(2);
  }

  EXPECT_EQ(stream.flags(), original_flags);
  EXPECT_EQ(stream.precision(), original_precision);
}

TEST(StreamStateGuardTest, RestoresFillCharacter) {
  std::ostringstream stream;
  stream.fill('_');

  const auto original_fill = stream.fill();

  {
    const ROOThelpers::StreamStateGuard guard(stream);
    stream.fill('*');
  }

  EXPECT_EQ(stream.fill(), original_fill);
}

TEST(StreamStateGuardTest, NestedGuardsRestorePreviousState) {
  std::ostringstream stream;
  stream << std::scientific << std::setprecision(6);

  const auto outer_flags = stream.flags();
  const auto outer_precision = stream.precision();

  {
    const ROOThelpers::StreamStateGuard outer_guard(stream);
    stream << std::fixed << std::setprecision(3);

    const auto inner_flags = stream.flags();
    const auto inner_precision = stream.precision();

    {
      const ROOThelpers::StreamStateGuard inner_guard(stream);
      stream << std::defaultfloat << std::setprecision(1);
    }

    EXPECT_EQ(stream.flags(), inner_flags);
    EXPECT_EQ(stream.precision(), inner_precision);
  }

  EXPECT_EQ(stream.flags(), outer_flags);
  EXPECT_EQ(stream.precision(), outer_precision);
}
