#include "interrupt_manager.h"

#include <asm/interrupt.h>
#include <asm/memset.h>

#include <cstdint>
#include <utility>

#include "mgba/logger.h"
#include "registers/display.h"
#include "registers/registers.h"

namespace toncar {

namespace {

/// REG_IE
class InterruptEnable : public Register<InterruptEnable, uint16_t, 0x0200> {
 public:
  InterruptEnable& EnableIrqVBlank() { return SetBit<std::to_underlying(Interrupt::VBlank)>(); }
  InterruptEnable& EnableIrqHBlank() { return SetBit<std::to_underlying(Interrupt::HBlank)>(); }
  InterruptEnable& EnableIrqVCount() { return SetBit<std::to_underlying(Interrupt::VCount)>(); }

  InterruptEnable& DisableIrqVBlank() { return ClearBit<std::to_underlying(Interrupt::VBlank)>(); }
  InterruptEnable& DisableIrqHBlank() { return ClearBit<std::to_underlying(Interrupt::HBlank)>(); }
  InterruptEnable& DisableIrqVCount() { return ClearBit<std::to_underlying(Interrupt::VCount)>(); }
};

/// REG_IF
class InterruptRequestFlags : public Register<InterruptRequestFlags, uint16_t, 0x0202> {
 public:
  InterruptRequestFlags& AckIrqVBlank() { return Set(1 << std::to_underlying(Interrupt::VBlank)); }
  InterruptRequestFlags& AckIrqHBlank() { return Set(1 << std::to_underlying(Interrupt::HBlank)); }
  InterruptRequestFlags& AckIrqVCount() { return Set(1 << std::to_underlying(Interrupt::VCount)); }

  InterruptRequestFlags& AckAllIrq() { return Or(0); }
};

/// REG_IME
class InterruptMasterEnable : public Register<InterruptMasterEnable, uint16_t, 0x0208> {
 public:
  InterruptMasterEnable& Enable() { return Set(1); }
  InterruptMasterEnable& Disable() { return Set(0); }
  [[nodiscard]] bool IsEnabled() const { return Get() == 1; }
};

void EnableIrq(Interrupt interrupt) {
  InterruptMasterEnable& reg_ime{InterruptMasterEnable::Instance()};
  DispStat& reg_dispstat{DispStat::Instance()};
  InterruptEnable& reg_ie{InterruptEnable::Instance()};

  bool ime_was_enabled{reg_ime.IsEnabled()};
  reg_ime.Disable();

  // TODO: could be templated but would imply putting everything in header. Evaluate what we want
  switch (interrupt) {
    case Interrupt::VBlank: {
      reg_dispstat.RequestHBlankInterrupt();
      reg_ie.EnableIrqVBlank();
      break;
    }
    case Interrupt::HBlank: {
      reg_dispstat.RequestHBlankInterrupt();
      reg_ie.EnableIrqHBlank();
      break;
    }
    case Interrupt::VCount: {
      reg_dispstat.RequestVCountInterrupt();
      reg_ie.EnableIrqVCount();
      break;
    }
    default: {
      MGBA_LOG_FATAL("Interrupt \"%d\" not supported yet", std::to_underlying(interrupt));
    }
  }

  if (ime_was_enabled) {
    reg_ime.Enable();
  } else {
    reg_ime.Disable();
  }
}

void DisableIrq(Interrupt interrupt) {
  InterruptMasterEnable& reg_ime{InterruptMasterEnable::Instance()};
  DispStat& reg_dispstat{DispStat::Instance()};
  InterruptEnable& reg_ie{InterruptEnable::Instance()};

  bool ime_was_enabled{reg_ime.IsEnabled()};
  reg_ime.Disable();

  switch (interrupt) {
    case Interrupt::VBlank: {
      reg_dispstat.CancelVBlankInterruptRequest();
      reg_ie.DisableIrqVBlank();
      break;
    }
    case Interrupt::HBlank: {
      reg_dispstat.CancelHBlankInterruptRequest();
      reg_ie.DisableIrqHBlank();
      break;
    }
    case Interrupt::VCount: {
      reg_dispstat.CancelVCountInterruptRequest();
      reg_ie.DisableIrqVCount();
      break;
    }
    default: {
      MGBA_LOG_FATAL("Interrupt \"%d\" not supported yet", std::to_underlying(interrupt));
    }
  }

  if (ime_was_enabled) {
    reg_ime.Enable();
  } else {
    reg_ime.Disable();
  }
}

/// REG_ISR_MAIN
const auto kRegIsrMain = reinterpret_cast<volatile Fnptr*>(0x03007FFC);

}  // namespace

InterruptManager::InterruptManager() {
  InterruptMasterEnable& ime{InterruptMasterEnable::Instance()};
  ime.Disable();

  // Clear interrupt table (just in case)
  memset32(kIsrTable.data(), 0, (kIsrTable.size() + 1) * sizeof(IrqRec) / 4);

  // TODO: support custom isr
  *kRegIsrMain = static_cast<Fnptr>(isr_master);

  ime.Enable();

  static_cast<void>(DisableIrq);
}

void InterruptManager::RequestInterrupt(Interrupt interrupt) { EnableIrq(interrupt); }

}  // namespace toncar
