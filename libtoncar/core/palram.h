#pragma once

#include <asm/memcpy.h>
#include <sprite.h>
#include <zone.h>

#include <cstdint>

#include "toncar.h"

namespace toncar {

class Palram final {
 private:
  template <uint16_t Offset>
  class Palbank final : public Zone<Palbank<Offset>, uint16_t, memory::kPalram + Offset, 256> {
   public:
    void LoadPalette(const Palette16* palette) {
      memcpy32(const_cast<uint16_t*>(this->BaseAddress()), palette, sizeof(Palette16));
    }
  };

 public:
  static Palram& Instance() {
    static Palram palram{};
    return palram;
  }

  Palbank<0x0000>& BackgroundsPalbank() { return backgrounds_palbank_; }
  Palbank<0x0200>& SpritesPalbank() { return sprites_palbank_; }

 private:
  Palbank<0x0000> backgrounds_palbank_{Palbank<0x0000>::Instance()};
  Palbank<0x0200> sprites_palbank_{Palbank<0x0200>::Instance()};

  Palram() = default;
};

}  // namespace toncar
