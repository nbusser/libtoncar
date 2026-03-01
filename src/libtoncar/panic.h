#pragma once

#include <string_view>

namespace toncar {

void Panic(std::string_view condition, std::string_view file, int32_t line);

// NOLINTBEGIN(cppcoreguidelines-macro-usage)
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
