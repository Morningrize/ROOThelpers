#ifndef IOUTILS_H
#define IOUTILS_H

#include <string>
#include <vector>

constexpr int one = 1;

namespace ROOThelpers {
std::vector<std::string> parse_lines(const std::string &input_file);
std::vector<std::string> list_directory(const std::string &directory,
                                        const std::string &extension = "");
} // namespace ROOThelpers
#endif