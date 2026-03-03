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
#define MGBA_LOG_DEBUG(...)                                    \
  do {                                                         \
    MGBA_LOG(toncar::mgba::Logger::Level::Debug, __VA_ARGS__); \
  } while (0)
#define MGBA_LOG_INFO(...)                                    \
  do {                                                        \
    MGBA_LOG(toncar::mgba::Logger::Level::Info, __VA_ARGS__); \
  } while (0)
#define MGBA_LOG_WARNING(...)                                    \
  do {                                                           \
    MGBA_LOG(toncar::mgba::Logger::Level::Warning, __VA_ARGS__); \
  } while (0)
#define MGBA_LOG_ERROR(...)                                    \
  do {                                                         \
    MGBA_LOG(toncar::mgba::Logger::Level::Error, __VA_ARGS__); \
  } while (0)
#define MGBA_LOG_FATAL(...)                                    \
  do {                                                         \
    MGBA_LOG(toncar::mgba::Logger::Level::Fatal, __VA_ARGS__); \
  } while (0)
#else
#define MGBA_LOG(level, ...) \
  do { /* noop */            \
  } while (0)
#define MGBA_LOG_DEBUG(...) \
  do { /* noop */           \
  } while (0)
#define MGBA_LOG_INFO(...) \
  do { /* noop */          \
  } while (0)
#define MGBA_LOG_WARNING(...) \
  do { /* noop */             \
  } while (0)
#define MGBA_LOG_ERROR(...) \
  do { /* noop */           \
  } while (0)
#define MGBA_LOG_FATAL(...) \
  do { /* noop */           \
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

  enum class Level : uint8_t { Fatal = 0, Error = 1, Warning = 2, Info = 3, Debug = 4 };

  /// Please call via `MGBA_LOG` to optimize production builds.
  static void Log(Level level, const char* log_string, ...) __attribute__((format(printf, 2, 3)));

 private:
  Logger();

  static void EnsureInstanceExists() { static Logger logger{}; }

  const devoptab_t dotab_stdout_;
  const devoptab_t dotab_stderr_;
};

}  // namespace toncar::mgba
