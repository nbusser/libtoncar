#pragma once

#include <asm/interrupt.h>

#include <cstdint>

#include "attributes.h"

namespace toncar {

enum class Interrupt : uint8_t {
  VBlank = 0,
  HBlank = 1,
  VCount = 2,
  // Other interrupts are not yet supported
};

/// High level interrupt manager.
/// Takes care of enabling interrupts and to manage handlers.
class InterruptManager {
 public:
  InterruptManager& EnableInterrupt(Interrupt interrupt);

  InterruptManager& DisableInterrupt(Interrupt interrupt);

  /// Set a handler for a specific `interrupt` type.
  /// If another handler is already registers, overrides it.
  InterruptManager& SetInterruptHandler(Interrupt interrupt, Fnptr handler TONCAR_NONNULL);

  InterruptManager& DeleteInterruptHandler(Interrupt interrupt);

  static InterruptManager& GetInstance() {
    static InterruptManager instance{};
    return instance;
  }

 private:
  InterruptManager();
};

}  // namespace toncar
