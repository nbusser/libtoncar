#pragma once

#include <cstdint>
#include <span>

namespace toncar {
using Fnptr = void (*)();

struct IrqRec {
  uint32_t flag{};
  Fnptr isr{nullptr};
};

static constexpr uint32_t kInterruptionMax{14};

// Referenced in assembly file. Use extern C to avoid name mangling.
// NOLINT(*-avoid-c-arrays): accessed in assembly files
extern "C" IrqRec isr_table[kInterruptionMax];

/// C++ wrapper around `isr_table`.
inline const std::span<IrqRec, kInterruptionMax> kIsrTable{static_cast<IrqRec*>(isr_table),
                                                           kInterruptionMax};

// NOLINTNEXTLINE(readability-identifier-naming)
extern "C" void isr_master();

}  // namespace toncar
