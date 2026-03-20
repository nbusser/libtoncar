#pragma once

#include <string_view>

#include "mgba/logger.h"

namespace toncar {

void Panic(std::string_view condition, std::string_view file, int32_t line);

// NOLINTBEGIN(cppcoreguidelines-macro-usage)
#ifndef NDEBUG
#define MGBA_ASSERT(cond)                                                                       \
  do {                                                                                          \
    if (!(cond)) {                                                                              \
      MGBA_LOG_FATAL("Assertion failed: \"%s\"\n\nLocation: %s:%d", #cond, __FILE__, __LINE__); \
    }                                                                                           \
  } while (0)
#else
#define MGBA_ASSERT(cond) \
  do { /* noop */         \
  } while (0)
#endif

#ifndef NDEBUG
#define GBA_ASSERT(cond)                \
  do {                                  \
    if (!(cond)) {                      \
      Panic(#cond, __FILE__, __LINE__); \
    }                                   \
  } while (0)
#else
#define GBA_ASSERT(cond) \
  do { /* noop */        \
  } while (0)
#endif
// NOLINTEND(cppcoreguidelines-macro-usage)

}  // namespace toncar
