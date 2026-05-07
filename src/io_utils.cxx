#include "io_utils.h"

#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <vector>
#include <string>

namespace ROOThelpers {

std::vector<std::string> parse_lines(const std::string& input_file) {
  std::vector<std::string> lines;
  std::ifstream input_file_stream(input_file);
  if (!input_file_stream.is_open()) {
    throw std::runtime_error("Error: could not open file  '" + input_file +
                             "'\n");
  }
  std::string line;
  while (std::getline(input_file_stream, line)) {
    if (!line.empty()) {
      lines.push_back(line);
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