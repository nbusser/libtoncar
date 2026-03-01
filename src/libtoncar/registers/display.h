#pragma once

#include <cstdint>

#include "registers/registers.h"

namespace toncar {

/// REG_DISPCNT: Display control register.
class Dispcnt : public Register<Dispcnt, uint16_t, 0x00> {
 public:
  /// Bits 0-2: DCNT_MODE
  enum class Mode : uint16_t {
    DcntMode0 = 0x0000,
    DcntMode1 = 0x0001,
    DcntMode2 = 0x0002,
    DcntMode3 = 0x0003,
    DcntMode4 = 0x0004,
    DcntMode5 = 0x0005
  };

  /// Bits 8-12: DCNT_LAYER
  enum class Layer : uint16_t {
    DcntBg0 = 1 << 8,   // 0x0100
    DcntBg1 = 1 << 9,   // 0x0200
    DcntBg2 = 1 << 10,  // 0x0400
    DcntBg3 = 1 << 11,  // 0x0800
    DcntObj = 1 << 12   // 0x1000
  };

  Dispcnt& SetLayer(Layer layer) { return Or(static_cast<uint16_t>(layer)); }

  Dispcnt& ClearLayer(Layer layer) { return And(~static_cast<uint16_t>(layer)); }

  Dispcnt& FlushLayers() { return And(0x0007); }

  [[nodiscard]] bool HasLayer(Layer layer) const {
    return GetAnd(static_cast<uint16_t>(layer)) != 0U;
  }

  Dispcnt& Reset() { return FlushLayers().SetMode(Mode::DcntMode0).SetLayer(Layer::DcntBg0); }

  [[nodiscard]] Mode GetMode() const { return static_cast<Mode>(GetAnd(0x0007)); }
  Dispcnt& SetMode(Mode mode) {
    return And(static_cast<uint16_t>(~0x0007U)).Or(static_cast<uint16_t>(mode));
  }
};

/// REG_DISPSTAT: Display stat register.
/// TODO: could be more generic, but we will not over-engineer yet.
class DispStat : public Register<DispStat, uint16_t, 0x04> {
 public:
  [[nodiscard]] bool IsInVBlank() const { return HasBit<Fields::StatInVbl>(); }

  [[nodiscard]] bool IsInHBlank() const { return HasBit<Fields::StatInHbl>(); }

  [[nodiscard]] bool IsVCountTrigger() const { return HasBit<Fields::StatInVct>(); }

  DispStat& RequestVBlankInterrupt() { return SetBit<Fields::StatVlcIrq>(); }

  DispStat& RequestHBlankInterrupt() { return SetBit<Fields::StatHblIrq>(); }

  DispStat& RequestVCountInterrupt() { return SetBit<Fields::StatVctIrq>(); }

 private:
  enum Fields : uint8_t {
    StatInVbl = 0,
    StatInHbl = 1,
    StatInVct = 2,
    StatVlcIrq = 3,
    StatHblIrq = 4,
    StatVctIrq = 5,
  };
};

}  // namespace toncar
