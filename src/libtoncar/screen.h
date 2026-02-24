#pragma once

#include <cstdint>

#include "toncar.h"

namespace toncar {
class Screen : public Zone<Screen, uint16_t, memory::kVram> {
 public:
  static Screen& WritePixel(uint8_t x, uint8_t y, Color15 color) {
    return Set(y * kWidth + x, color.Value());
  }

  static constexpr uint32_t kWidth{240};
  static constexpr uint32_t kHeight{160};
};
}  // namespace toncar
