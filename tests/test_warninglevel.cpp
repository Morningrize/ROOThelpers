#include <iostream>
#include <sstream>
#include <stdexcept>
#include <streambuf>
#include <string>

#include <gtest/gtest.h>

#include "WarningLevel.h"

class WarningLevelTest : public ::testing::Test {
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

TEST_F(WarningLevelTest, DefaultWarningLevel) {
  EXPECT_EQ(ROOThelpers::get_warning_level(), ROOThelpers::WarningLevel::WARN);
}

TEST_F(WarningLevelTest, SetAndGet) {
  ROOThelpers::set_warning_level(ROOThelpers::WarningLevel::SILENT);
  EXPECT_EQ(ROOThelpers::get_warning_level(),
            ROOThelpers::WarningLevel::SILENT);

  ROOThelpers::set_warning_level(ROOThelpers::WarningLevel::WARN);
  EXPECT_EQ(ROOThelpers::get_warning_level(), ROOThelpers::WarningLevel::WARN);

  ROOThelpers::set_warning_level(ROOThelpers::WarningLevel::ERROR);
  EXPECT_EQ(ROOThelpers::get_warning_level(), ROOThelpers::WarningLevel::ERROR);
}

TEST_F(WarningLevelTest, HandleWarningSilent) {
  ROOThelpers::set_warning_level(ROOThelpers::WarningLevel::SILENT);
  EXPECT_NO_THROW(
      ROOThelpers::handle_warning("This message should not appear"));
  EXPECT_TRUE(test_stream.str().empty());
}

TEST_F(WarningLevelTest, HandleWarningWarn) {
  ROOThelpers::set_warning_level(ROOThelpers::WarningLevel::WARN);
  EXPECT_NO_THROW(ROOThelpers::handle_warning("Warning message"));
  EXPECT_NE(test_stream.str().find("Warning message"), std::string::npos);
}

TEST_F(WarningLevelTest, HandleWarningError) {
  ROOThelpers::set_warning_level(ROOThelpers::WarningLevel::ERROR);
  EXPECT_THROW(ROOThelpers::handle_warning("Test error"), std::runtime_error);
}