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

void RestoreIme(bool was_enabled) {
  InterruptMasterEnable& reg_ime{InterruptMasterEnable::Instance()};
  if (was_enabled) {
    reg_ime.Enable();
  } else {
    reg_ime.Disable();
  }
}

void EnableIrq(Interrupt interrupt) {
  DispStat& reg_dispstat{DispStat::Instance()};
  InterruptEnable& reg_ie{InterruptEnable::Instance()};

  // TODO: could be templated but would imply putting everything in header. Evaluate what we want
  switch (interrupt) {
    case Interrupt::VBlank: {
      reg_dispstat.RequestVBlankInterrupt();
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
}

void DisableIrq(Interrupt interrupt) {
  DispStat& reg_dispstat{DispStat::Instance()};
  InterruptEnable& reg_ie{InterruptEnable::Instance()};

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
}

Fnptr AddIrq(Interrupt interrupt, Fnptr isr) {
  InterruptMasterEnable& reg_ime{InterruptMasterEnable::Instance()};

  bool ime_was_enabled{reg_ime.IsEnabled()};
  reg_ime.Disable();

  // Finds in the `kIsrTable` either:
  // - a free slot (flag == 0)
  // - another entry for the same IRQ

  const auto irq_flag = static_cast<uint16_t>(1 << std::to_underlying(interrupt));

  IrqRec* entry_slot{nullptr};

  for (IrqRec& entry : kIsrTable) {
    // Empty slot
    if (entry.flag == 0) {
      entry_slot = &entry;
      break;
    }
    // Same Interrupt to be replaced
    if (entry.flag == irq_flag) {
      entry_slot = &entry;
      break;
    }
  }

  // No entry to replace and no more slot available. Crashing.
  if (entry_slot == nullptr) {
    MGBA_LOG_FATAL("No available slot for adding interrupt \"%d\"", std::to_underlying(interrupt));
    GBA_ASSERT(false);
  }

  // Replacing the found entry with our information.
  Fnptr old_isr = entry_slot->isr;
  *entry_slot = IrqRec{
      .flag = irq_flag,
      .isr = isr,
  };

  RestoreIme(ime_was_enabled);

  return old_isr;
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
}

InterruptManager& InterruptManager::AddInterruptHandler(Interrupt interrupt, Fnptr arm_handler) {
  AddIrq(interrupt, arm_handler);
  return *this;
}

InterruptManager& InterruptManager::EnableInterrupt(Interrupt interrupt) {
  EnableIrq(interrupt);
  return *this;
}

InterruptManager& InterruptManager::DisableInterrupt(Interrupt interrupt) {
  DisableIrq(interrupt);
  return *this;
}

}  // namespace toncar
