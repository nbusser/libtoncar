#pragma once

#include <cstdint>
#include <span>

namespace toncar {
using Fnptr = void (*)();

struct IrqRec {
  uint32_t flag{};
  Fnptr isr{nullptr};
};

// Matches libtonc original's `II_MAX`.
static constexpr uint32_t kInterruptionMax{15};

// libtonc's priority IRQ tables. Filled by `InterruptManager` and read by the assembly code.
// NOLINT(*-avoid-c-arrays): accessed in assembly files
extern "C" IrqRec isr_table[kInterruptionMax];

/// C++ wrapper around `isr_table`.
inline const std::span<IrqRec, kInterruptionMax> kIsrTable{static_cast<IrqRec*>(isr_table),
                                                           kInterruptionMax};

// NOLINTNEXTLINE(readability-identifier-naming)
extern "C" void isr_master();

}  // namespace toncar
