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

class InterruptManager {
 public:
  InterruptManager& EnableInterrupt(Interrupt interrupt);

  InterruptManager& DisableInterrupt(Interrupt interrupt);

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
