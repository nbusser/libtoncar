#pragma once

#include <cstdint>

#include "colors.h"
#include "src/mgba-debug/mgba.h"
#include "toncar.h"

namespace toncar {

class Screen : public Zone<Screen, uint16_t, memory::kVram, 38400> {
 public:
  static Screen& Mode3WritePixel(uint8_t x, uint8_t y, Color15 color) {
    mgba_printf(MGBA_LOG_INFO, "Salut la compagnie\n");
    return Set((y * kWidth) + x, color.Value());
  }

  static constexpr uint32_t kWidth{240};
  static constexpr uint32_t kHeight{160};
};

}  // namespace toncar
