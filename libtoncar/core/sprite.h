#pragma once

#include <colors.h>

#include <array>
#include <cstdint>
#include <span>

namespace toncar {

using Palette16 = std::array<std::uint16_t, 16>;

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

  constexpr Sprite(const Palette16& palette, std::span<const uint8_t> data, Size size)
      : palette_{&palette}, data_{data}, size_{size} {
    static_cast<void>(palette_);
    static_cast<void>(data_);
    static_cast<void>(size_);
  }

 private:
  /// Stored in ROM. Lifetime static.
  const Palette16* palette_;
  /// Stored in ROM. Lifetime static.
  const std::span<const uint8_t> data_;
  Size size_;
};

class Tag {
 public:
  Tag(std::span<const Sprite*> sprites) : sprites_(sprites) {}

 private:
  const std::span<const Sprite*> sprites_;
  // TODO: direction
};

}  // namespace toncar
