#pragma once

#include <cstdint>

#include "asm/memcpy.h"
#include "sprite.h"
#include "toncar.h"

namespace toncar {

class Palram final {
 private:
  class Palbank final {
   public:
    constexpr explicit Palbank(size_t offset) : offset_{offset} {}

    void LoadPalette(const Palette16* palette) {
      MGBA_ASSERT(next_palette_id_ < kMaxPaletteId);
      memcpy32(reinterpret_cast<uint16_t*>(kBaseAddress + offset_ +
                                           (next_palette_id_ * palette->size() * sizeof(uint16_t))),
               palette,
               sizeof(Palette16) / 4);
      ++next_palette_id_;
    }

   private:
    static constexpr uintptr_t kBaseAddress{memory::kPalram};
    size_t offset_;

    static constexpr uint16_t kMaxPaletteId{16};
    uint16_t next_palette_id_{0};
  };

 public:
  static Palram& Instance() {
    static Palram palram{};
    return palram;
  }

  Palbank& BackgroundsPalbank() { return backgrounds_palbank_; }
  Palbank& SpritesPalbank() { return sprites_palbank_; }

 private:
  Palram() = default;

  Palbank backgrounds_palbank_{Palbank(0x0000)};
  Palbank sprites_palbank_{Palbank(0x0200)};
};

}  // namespace toncar
