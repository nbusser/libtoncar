#pragma once

#include <panic.h>

#include <cstdint>
#include <utility>

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
  /// One hot bit.
  enum class Layer : uint8_t {
    DcntBg0 = 8,   // 0x0100
    DcntBg1 = 9,   // 0x0200
    DcntBg2 = 10,  // 0x0400
    DcntBg3 = 11,  // 0x0800
    DcntObj = 12   // 0x1000
  };

  template <Layer layer>
  Dispcnt& SetLayer() {
    return SetBit<std::to_underlying(layer)>();
  }

  template <Layer layer>
  Dispcnt& ClearLayer() {
    return ClearBit<std::to_underlying(layer)>();
  }

  template <Layer layer>
  [[nodiscard]] bool HasLayer() const {
    return HasLayer<std::to_underlying(layer)>();
  }

  Dispcnt& FlushLayers() { return And(0x0007); }

  Dispcnt& Reset() { return FlushLayers().SetMode(Mode::DcntMode0).SetLayer<Layer::DcntBg0>(); }

  [[nodiscard]] Mode GetMode() const { return static_cast<Mode>(GetAnd(0x0007)); }
  Dispcnt& SetMode(Mode mode) {
    return And(static_cast<uint16_t>(~0x0007U)).Or(static_cast<uint16_t>(mode));
  }
};

/// REG_DISPSTAT: Display stat register.
/// TODO: could be more generic, but we will not over-engineer yet.
class DispStat : public Register<DispStat, uint16_t, 0x04> {
 public:
  [[nodiscard]] bool IsInVBlank() const { return HasBit<std::to_underlying(Fields::StatInVbl)>(); }

  [[nodiscard]] bool IsInHBlank() const { return HasBit<std::to_underlying(Fields::StatInHbl)>(); }

  [[nodiscard]] bool IsVCountTrigger() const {
    return HasBit<std::to_underlying(Fields::StatInVct)>();
  }

  DispStat& RequestVBlankInterrupt() { return SetBit<std::to_underlying(Fields::StatVlcIrq)>(); }

  DispStat& RequestHBlankInterrupt() { return SetBit<std::to_underlying(Fields::StatHblIrq)>(); }

  DispStat& RequestVCountInterrupt() { return SetBit<std::to_underlying(Fields::StatVctIrq)>(); }

 private:
  enum class Fields : uint8_t {
    StatInVbl = 0,
    StatInHbl = 1,
    StatInVct = 2,
    StatVlcIrq = 3,
    StatHblIrq = 4,
    StatVctIrq = 5,
  };
};

/// REG_VCOUNT: Display vertical count register.
class VCount : public Register<VCount, uint16_t, 0x06> {
 public:
  /// Gets the VCount value (between 0 and 227).
  [[nodiscard]] uint16_t Value() const {
    const uint16_t value{GetAnd(0x00FF)};
    GBA_ASSERT(value < 228);
    return value;
  }
};

}  // namespace toncar
