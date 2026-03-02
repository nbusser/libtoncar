#include "interrupt_manager.h"

#include <asm/interrupt.h>
#include <asm/memset.h>

#include <cstdint>
#include <utility>

#include "attributes.h"
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
/// Unused. libtonc's assembly ARM routines already acknowledge interruptions.
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

/// Disables the `REG_IME` on construction.
/// Restores the previous state of `REG_IME` on destruction.
/// Ensures that no interruption ever happen when we are manipulating interruption handler tables.
class RegImeLock {
 public:
  RegImeLock() : reg_ime_{InterruptMasterEnable::Instance()}, was_enabled_{reg_ime_.IsEnabled()} {
    reg_ime_.Disable();
  }

  ~RegImeLock() {
    if (was_enabled_) {
      reg_ime_.Enable();
    }
  }

  RegImeLock(const RegImeLock&) = delete;
  RegImeLock(RegImeLock&&) = delete;
  RegImeLock& operator=(const RegImeLock&) = delete;
  RegImeLock& operator=(RegImeLock&&) = delete;

 private:
  InterruptMasterEnable& reg_ime_;
  bool was_enabled_;
};

/// Sets the proper flags in `REG_DISPSTAT` and `REG_IE` to enable the interrupt.
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

/// Clears the proper flags in `REG_DISPSTAT` and `REG_IE` to disable the interrupt.
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

/// Interfaces with libtonc's `isr_master()`.
/// Finds an entry for `interrupt` in `kIsrTable` and erase it with a new one.
/// Appends the handler as a new entry if not found.
Fnptr SetIrq(Interrupt interrupt, Fnptr isr) {
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
  }

  // Replacing the found entry with our information.
  Fnptr old_isr = entry_slot->isr;
  *entry_slot = IrqRec{
      .flag = irq_flag,
      .isr = isr,
  };

  return old_isr;
}

/// Interfaces with libtonc's `isr_master()`.
/// Finds the entry for `interrupt` in `kIsrTable` and deletes it.
/// Do nothing if not found.
void DeleteIrq(Interrupt interrupt) {
  const auto irq_flag = static_cast<uint16_t>(1 << std::to_underlying(interrupt));

  for (uint32_t i{0}; i < kIsrTable.size(); ++i) {
    const IrqRec& current_entry{kIsrTable[i]};

    // Empty slot -> no more IRQs
    if (current_entry.flag == 0) {
      break;
    }
    // Same flag: need to delete by shifting all the right elements to the left
    if (current_entry.flag == irq_flag) {
      uint32_t j{i};
      for (; j < kIsrTable.size() - 1; ++j) {
        kIsrTable[j] = kIsrTable[j + 1];
        // Empty slot -> no more IRQs
        if (kIsrTable[j + 1].flag == 0) {
          break;
        }
      }
      // If the table was full, we need to reset the last entry (duplicated).
      if (j == kIsrTable.size() - 1) {
        kIsrTable[j] = IrqRec{};
      }
      // Finish
      break;
    }
  }
}

/// REG_ISR_MAIN
const auto kRegIsrMain = reinterpret_cast<volatile Fnptr*>(0x03007FFC);

}  // namespace

/// Private CTor created by `GetInstance()`.
/// Clears the `kIsrTable`, sets `isr_master` on `REG_ISR_MAIN` and  enable `REG_IME`.
InterruptManager::InterruptManager() {
  InterruptMasterEnable& ime{InterruptMasterEnable::Instance()};
  ime.Disable();

  // Clear interrupt table (with sentinel).
  memset32(kIsrTable.data(), 0, (kIsrTable.size() + 1) * sizeof(IrqRec) / 4);

  // TODO: support custom isr
  *kRegIsrMain = static_cast<Fnptr>(isr_master);

  ime.Enable();
}

InterruptManager& InterruptManager::EnableInterrupt(Interrupt interrupt) {
  const RegImeLock lock{};
  EnableIrq(interrupt);
  return *this;
}

InterruptManager& InterruptManager::DisableInterrupt(Interrupt interrupt) {
  const RegImeLock lock{};
  DisableIrq(interrupt);
  return *this;
}

InterruptManager& InterruptManager::SetInterruptHandler(Interrupt interrupt,
                                                        Fnptr handler TONCAR_NONNULL) {
  const RegImeLock lock{};
  SetIrq(interrupt, handler);
  return *this;
}

InterruptManager& InterruptManager::DeleteInterruptHandler(Interrupt interrupt) {
  const RegImeLock lock{};
  DeleteIrq(interrupt);
  return *this;
}

}  // namespace toncar
