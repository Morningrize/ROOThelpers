#ifndef STREAM_STATE_GUARD_H
#define STREAM_STATE_GUARD_H

#include <exception>
#include <ios>
#include <ostream>

namespace ROOThelpers {

class StreamStateGuard {
public:
  explicit StreamStateGuard(std::ostream &stream)
      : stream_(stream), state_(nullptr) {
    state_.copyfmt(stream_);
  }

  ~StreamStateGuard() noexcept {
    try {
      stream_.copyfmt(state_);
    } catch (...) {
      [[maybe_unused]] const std::exception_ptr ignored =
          std::current_exception();
    }
  }

  StreamStateGuard(const StreamStateGuard &) = delete;
  StreamStateGuard &operator=(const StreamStateGuard &) = delete;
  StreamStateGuard(StreamStateGuard &&) = delete;
  StreamStateGuard &operator=(StreamStateGuard &&) = delete;

private:
  std::ostream &stream_;
  std::ios state_;
};

} // namespace ROOThelpers

#endif
