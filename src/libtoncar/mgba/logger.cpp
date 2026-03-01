#include "mgba/logger.h"

#include <sys/iosupport.h>

#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <string_view>

#include "src/mgba-debug/mgba.h"

namespace toncar::mgba {

Logger::Logger() {
  mgba_open();
  mgba_console_open();
}

Logger::~Logger() { mgba_close(); }

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
void Logger::Log(Level level, std::string_view message) {
  // TODO: check message is null-terminated.
  // NOLINTNEXTLINE(bugprone-suspicious-stringview-data-usage)
  mgba_printf(static_cast<int32_t>(level), message.data());
}

}  // namespace toncar::mgba
