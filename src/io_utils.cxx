#include "io_utils.h"

#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <vector>
#include <string>
#include <cctype>
#include <algorithm>
#include <string_view>

namespace ROOThelpers {

std::string_view trim(const std::string &untrimmed_string) {
  auto not_a_space = [] (unsigned char cha) {
    return !(std::isspace(cha));
  };
  auto first = std::ranges::find_if(untrimmed_string, not_a_space);
  auto last = std::ranges::find_if(untrimmed_string.rbegin(),untrimmed_string.rend(), not_a_space).base();
  if (first >= last) {return {};}
  return std::string_view{first, last};
}

std::vector<std::string> parse_lines(const std::string& input_file) {
  std::vector<std::string> lines;
  std::ifstream input_file_stream(input_file);
  if (!input_file_stream.is_open()) {
    throw std::runtime_error("Error: could not open file  '" + input_file +
                             "'\n");
  }
  std::string line;
  while (std::getline(input_file_stream, line)) {
    auto trimmed = trim(line);
    if (!trimmed.empty()) {
      lines.emplace_back(trimmed);
    }
  }
  return lines;
}

std::vector<std::string> list_directory(const std::string& directory, const std::string& extension) {
  if (!std::filesystem::exists(directory)) {
    throw std::runtime_error("Directory does not exist: " + directory);
  }
  std::vector<std::string> files;
  for (const auto& entry : std::filesystem::directory_iterator(directory)) {
    if (entry.is_regular_file()) {
      const auto& path = entry.path();
      if (!extension.empty() && path.extension() != extension) {
        continue;
      }
        files.push_back(path.string());
    }
  }
  return files;
}

}