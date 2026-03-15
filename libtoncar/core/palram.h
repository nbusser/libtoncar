#pragma once

#include <asm/memcpy.h>
#include <sprite.h>
#include <zone.h>

#include <cstdint>

#include "toncar.h"

namespace toncar {

class Palram final {
 private:
  class Palbank final {
   public:
    constexpr explicit Palbank(size_t offset) : offset_{offset} {}

    void LoadPalette(const Palette16* palette) const {
      memcpy32(reinterpret_cast<uint16_t*>(kBaseAddress + offset_), palette, sizeof(Palette16) / 4);
    }

   private:
    static constexpr uintptr_t kBaseAddress{memory::kPalram};
    size_t offset_;
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
