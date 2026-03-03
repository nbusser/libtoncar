#pragma once

#include <cstdint>
#include <span>

namespace toncar {
using Fnptr = void (*)();

struct IrqRec {
  uint32_t flag{};  // 0 means no-entry
  Fnptr isr{nullptr};
};

/// Matches libtonc original's `II_MAX`.
static constexpr uint32_t kInterruptionMax{14};

/// libtonc's priority IRQ tables.
///
/// This table stores the interrupt handlers.
/// Each handler is identified via the member `flag`, which is  a one-hot bit of the interrupt ID.
/// The interrupt ID is tp be found on enum `Interrupt` of `interrup_manager.h`.
/// For example, interrupt `VCount`'s flag is `1 << 3 == 8`.
///
/// To match `isr_master`'s behavior, the table contains a contiguous sequence of valid entries,
/// followed by a single sentinel entry with `flag == 0`.
/// It can technically contain multiple times the same flag, but only the first occurrence will be
/// ever executed.
///
/// The table is written by `InterruptManager` and read by `isr_master` assembly ARM code on
/// interruption.
extern "C" IrqRec isr_table[kInterruptionMax + 1];  // NOLINT(*-avoid - c - arrays)

/// C++ wrapper around `isr_table`. Does not include the sentinel value.
inline const std::span<IrqRec, kInterruptionMax> kIsrTable{static_cast<IrqRec*>(isr_table),
                                                           kInterruptionMax};

/// libtonc's ARM routine reading the `isr_table` and calling the proper handler.
/// It also resets the proper `REG_IF` bit.
extern "C" void isr_master();  // NOLINT(readability-identifier-naming)

}  // namespace toncar
