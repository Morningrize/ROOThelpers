#ifndef ROOTHELPERS_WARNINGS_H
#define ROOTHELPERS_WARNINGS_H

#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>

namespace ROOThelpers {

enum class WarningLevel : std::uint8_t { SILENT, WARN, ERROR };

inline WarningLevel &warning_level() {
  static WarningLevel level{WarningLevel::WARN};
  return level;
}

inline WarningLevel get_warning_level() { return warning_level(); }

inline void set_warning_level(WarningLevel level) { warning_level() = level; }

inline void handle_warning(const std::string &msg) {
  switch (get_warning_level()) {
  case WarningLevel::SILENT:
    break;
  case WarningLevel::WARN:
    std::clog << "Warning: " << msg << '\n';
    break;
  case WarningLevel::ERROR:
    throw std::runtime_error(msg);
  }
}

} // namespace ROOThelpers

#endif