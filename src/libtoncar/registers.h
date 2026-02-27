#pragma once

#include <cstdint>

#include "toncar.h"

namespace toncar {

class Dispcnt : public Register<Dispcnt, uint16_t, memory::kIo> {
 public:
  enum class Mode : uint16_t {
    DcntMode0 = 0x0000,
    DcntMode1 = 0x0001,
    DcntMode2 = 0x0002,
    DcntMode3 = 0x0003,
    DcntMode4 = 0x0004,
    DcntMode5 = 0x0005
  };

  enum class Layer : uint16_t {
    DcntBg0 = 1 << 8,   // 0x0100
    DcntBg1 = 1 << 9,   // 0x0200
    DcntBg2 = 1 << 10,  // 0x0400
    DcntBg3 = 1 << 11,  // 0x0800
    DcntObj = 1 << 12   // 0x1000
  };

  static Dispcnt& SetLayer(Layer layer) { return Or(static_cast<uint16_t>(layer)); }

  static Dispcnt& ClearLayer(Layer layer) { return And(~static_cast<uint16_t>(layer)); }

  static Dispcnt& FlushLayers() { return And(0x0007); }

  static bool HasLayer(Layer layer) { return GetAnd(static_cast<uint16_t>(layer)) != 0U; }

  static Mode GetMode() { return static_cast<Mode>(GetAnd(0x0007)); }
  static Dispcnt& SetMode(Mode mode) {
    return And(static_cast<uint16_t>(~0x0007U)).Or(static_cast<uint16_t>(mode));
  }

  static Dispcnt& Reset() {
    return FlushLayers().SetMode(Mode::DcntMode0).SetLayer(Layer::DcntBg0);
  }
};

}  // namespace toncar
