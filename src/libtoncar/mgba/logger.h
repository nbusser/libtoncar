#pragma once

#include <sys/iosupport.h>

#include <cstdint>

namespace toncar::mgba {

// NOLINTBEGIN(cppcoreguidelines-macro-usage)
#ifndef NDEBUG
#define MGBA_LOG(level, ...)                       \
  do {                                             \
    toncar::mgba::Logger::Log(level, __VA_ARGS__); \
  } while (0)
#else
#define MGBA_LOG(level, ...) \
  do { /* noop */            \
  } while (0)
#endif
// NOLINTEND(cppcoreguidelines-macro-usage)

class Logger final {
 public:
  ~Logger();
  Logger(const Logger&) = delete;
  Logger(Logger&&) = delete;
  Logger& operator=(const Logger&) = delete;
  Logger& operator=(Logger&&) = delete;

  enum class Level : uint8_t { Fatal = 0, Error = 1, Warn = 2, Info = 3, Debug = 4 };

  /// Please call via `MGBA_LOG` to optimize production builds.
  static void Log(Level level, const char* ptr, ...) __attribute__((format(printf, 2, 3)));

 private:
  Logger();

  static void EnsureInstanceExists() { static Logger logger{}; }

  const devoptab_t dotab_stdout_;
  const devoptab_t dotab_stderr_;
};

}  // namespace toncar::mgba
