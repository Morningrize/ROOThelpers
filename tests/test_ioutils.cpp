#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "io_utils.h"

class ParseLinesTest : public ::testing::Test {
protected:
  void SetUp() override {
    tmp_file = std::filesystem::temp_directory_path() / "test_parse_lines.txt";
  }
  void TearDown() override { std::filesystem::remove(tmp_file); }
  std::filesystem::path tmp_file; // NOLINT
};

class ListDirectoryTest : public ::testing::Test {
protected:
  void SetUp() override {
    tmp_dir = std::filesystem::temp_directory_path() / "test_list_directory";
    std::filesystem::create_directory(tmp_dir);
  }
  void TearDown() override { std::filesystem::remove_all(tmp_dir); }
  std::filesystem::path tmp_dir; // NOLINT
};

// ----------------------
// parse_lines
// ----------------------

TEST_F(ParseLinesTest, ThrowsForMissingFile) {
  EXPECT_THROW(ROOThelpers::parse_lines("/nonexistent/path/file.txt"),
               std::runtime_error);
}

TEST_F(ParseLinesTest, ReturnsEmptyForEmptyFile) {
  std::ofstream file_stream(tmp_file);
  file_stream.close();
  EXPECT_TRUE(ROOThelpers::parse_lines(tmp_file.string()).empty());
}

TEST_F(ParseLinesTest, ReturnsLinesFromFile) {
  std::ofstream file_stream(tmp_file);
  file_stream << "line1\nline2\nline3\n";
  file_stream.close();

  const auto lines = ROOThelpers::parse_lines(tmp_file.string());
  EXPECT_EQ(lines.size(), 3U);
  EXPECT_EQ(lines[0], "line1");
  EXPECT_EQ(lines[2], "line3");
}

TEST_F(ParseLinesTest, SkipsEmptyLines) {
  std::ofstream file_stream(tmp_file);
  file_stream << "line1\n\nline2\n";
  file_stream.close();

  const auto lines = ROOThelpers::parse_lines(tmp_file.string());
  EXPECT_EQ(lines.size(), 2U);
  EXPECT_EQ(lines[1], "line2");
}

// ----------------------
// list_directory
// ----------------------

TEST_F(ListDirectoryTest, ThrowsForMissingDirectory) {
  EXPECT_THROW(ROOThelpers::list_directory("/nonexistent/path"),
               std::runtime_error);
}

TEST_F(ListDirectoryTest, ReturnsEmptyForEmptyDirectory) {
  EXPECT_TRUE(ROOThelpers::list_directory(tmp_dir.string()).empty());
}

TEST_F(ListDirectoryTest, ReturnsEmptyWhenNoMatchingExtension) {
  std::ofstream(tmp_dir / "file1.txt").close();
  EXPECT_TRUE(ROOThelpers::list_directory(tmp_dir.string(), "root").empty());
}

TEST_F(ListDirectoryTest, ListsAllExtensionUnspecified) {
  std::ofstream(tmp_dir / "file1.txt").close();
  std::ofstream(tmp_dir / "file2.root").close();

  const auto files = ROOThelpers::list_directory(tmp_dir.string());
  EXPECT_EQ(files.size(), 2U);
}

TEST_F(ListDirectoryTest, FiltersFilesByExtension) {
  std::ofstream(tmp_dir / "file1.txt").close();
  std::ofstream(tmp_dir / "file2.root").close();
}