#pragma once

#include <colors.h>

#include <array>
#include <cstdint>

namespace toncar {

class Palette16 {
 public:
  constexpr Palette16(std::array<Color15, 16> colors) : colors_{colors} {
    static_cast<void>(colors_);
  }

 private:
  std::array<Color15, 16> colors_;
};

class Sprite {
 public:
  enum Size {
    S8x8 = 0b0000,
    S16x16 = 0b0001,
    S32x32 = 0b0010,
    S64x64 = 0b0011,

    S16x8 = 0b0100,
    S32x8 = 0b0101,
    S32x16 = 0b0110,
    S64x32 = 0b0111,

    S8x16 = 0b1000,
    S8x32 = 0b1001,
    S16x32 = 0b1010,
  };

  constexpr Sprite(const Palette16& palette, const uint8_t* data, Size size)
      : palette_{&palette}, data_{data}, size_{size} {
    static_cast<void>(palette_);
    static_cast<void>(data_);
    static_cast<void>(size_);
  }

 private:
  /// Stored in ROM. Lifetime static.
  const Palette16* palette_;
  /// Stored in ROM. Lifetime static.
  const uint8_t* data_;
  Size size_;
};

}  // namespace toncar
