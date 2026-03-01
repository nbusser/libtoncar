#pragma once

#include <cstdint>
#include <string_view>

namespace toncar::mgba {

class Logger final {
 public:
  ~Logger();
  Logger(const Logger&) = delete;
  Logger(Logger&&) = delete;
  Logger& operator=(const Logger&) = delete;
  Logger& operator=(Logger&&) = delete;

  static Logger& GetInstance() {
    static Logger logger{};
    return logger;
  }

  enum class Level : uint8_t { Fatal = 0, Error = 1, Warn = 2, Info = 3, Debug = 4 };

  void Log(Level level, std::string_view message);

 private:
  Logger();
};

}  // namespace toncar::mgba
