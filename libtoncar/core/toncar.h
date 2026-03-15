#pragma once

#include <cstdint>

namespace toncar::memory {

constexpr uintptr_t kIo{0x04000000};
constexpr uintptr_t kPalram{0x05000000};
constexpr uintptr_t kVram{0x06000000};

}  // namespace toncar::memory
